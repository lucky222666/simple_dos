#include <stdlib.h>
#include "pthread.h"
#include <unistd.h>

#include "threadpool.h"

#pragma comment(lib,"pthreadVC2.lib") 
/*�ز����٣����Ǹ��߱������ڱ����γɵ�.obj�ļ���.exe�ļ��м�һ����Ϣ��
ʹ�������������ӿ��ʱ��Ҫȥ��pthreadVC2.lib����⣬��Ҫ��ȥ�ұ�Ŀ⡣
��.exe�ļ���DLL Ҳ������д�������� pthreadVC2.dll��	 */


//�̳߳عرյķ�ʽ

typedef enum {
	immediate_shutdown = 1,
	graceful_shutdown = 2
} threadpool_shutdown_t;


//�̳߳�һ������Ķ���

typedef struct {
	void(*function)(void *);	   //ִ�к���
	void *argument;				  //��������
} threadpool_task_t;

/**
 * �̳߳صĽṹ����
 *  @var lock         �����ڲ������Ļ�����
 *  @var notify       �̼߳�֪ͨ����������
 *  @var threads      �߳����飬������ָ������ʾ�������� = ��Ԫ��ָ��
 *  @var thread_count �߳�����
 *  @var queue        �洢��������飬���������
 *  @var queue_size   ������д�С
 *  @var head         ����������׸�����λ�ã�ע���������������������δ��ʼ���еģ�
 *  @var tail         ������������һ���������һ��λ�ã�ע������������洢��head �� tail ָʾ����λ�ã�
 *  @var count        �����������������������ȴ����е�������
 *  @var shutdown     ��ʾ�̳߳��Ƿ�ر�
 *  @var started      ��ʼ���߳���
 */
struct threadpool_t {
	pthread_mutex_t lock;
	pthread_cond_t notify;

	pthread_t *threads;
	threadpool_task_t *queue;

	int thread_count;
	int queue_size;
	int head;
	int tail;
	int count;

	int shutdown;
	int started;
};

/**
 * @function void *threadpool_thread(void *threadpool)
 * @brief the worker thread
 * @param threadpool the pool which own the thread
 */
 /**
  * �̳߳���ÿ���߳����ܵĺ���
  * ���� static Ӧ��ֻΪ��ʹ����ֻ�ڱ��ļ�����Ч
  */
static void *threadpool_thread(void *threadpool);

int threadpool_free(threadpool_t *pool);

threadpool_t *threadpool_create(int thread_count, int queue_size, int flags)
{
	if (thread_count <= 0 || thread_count > MAX_THREADS || queue_size <= 0 || queue_size > MAX_QUEUE) {
		return NULL;
	}

	threadpool_t *pool;
	int i;

	/* �����ڴ洴���ڴ�ض��� */
	if ((pool = (threadpool_t *)malloc(sizeof(threadpool_t))) == NULL) {
		goto err;
	}

	/* ��ʼ�� */
	pool->thread_count = 0;
	pool->queue_size = queue_size;
	pool->head = pool->tail = pool->count = 0;
	pool->shutdown = pool->started = 0;

	/* Allocate thread and task queue */
	/* �����߳�������������������ڴ� */
	pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * thread_count);
	pool->queue = (threadpool_task_t *)malloc
	(sizeof(threadpool_task_t) * queue_size);


	/* ��ʼ������������������ */
	if ((pthread_mutex_init(&(pool->lock), NULL) != 0) ||
		(pthread_cond_init(&(pool->notify), NULL) != 0) ||
		(pool->threads == NULL) ||
		(pool->queue == NULL)) {
		goto err;
	}

	/* Start worker threads */
	/* ����ָ���������߳̿�ʼ���� */
	for (i = 0; i < thread_count; i++)
	{
		if (pthread_create(&(pool->threads[i]), NULL,
			threadpool_thread, (void*)pool) != 0)
		{
			threadpool_destroy(pool, 0);
			return NULL;
		}
		pool->thread_count++;
		pool->started++;
	}

	return pool;

err:
	if (pool) {
		threadpool_free(pool);
	}
	return NULL;
}

int threadpool_add(threadpool_t *pool, void(*function)(void *), void *argument, int flags)
{
	int err = 0;
	int next;

	if (pool == NULL || function == NULL) {
		return threadpool_invalid;
	}

	/* ������ȡ�û���������Ȩ */
	if (pthread_mutex_lock(&(pool->lock)) != 0) {
		return threadpool_lock_failure;
	}

	/* ������һ�����Դ洢 task ��λ�� */
	next = pool->tail + 1;
	next = (next == pool->queue_size) ? 0 : next;

	do {
		/* Are we full ? */
		/* ����Ƿ���������� */
		if (pool->count == pool->queue_size) {
			err = threadpool_queue_full;
			break;
		}

		/* Are we shutting down ? */
		/* ��鵱ǰ�̳߳�״̬�Ƿ�ر� */
		if (pool->shutdown) {
			err = threadpool_shutdown;
			break;
		}

		/* �� tail ��λ�÷��ú���ָ��Ͳ��������ӵ�������� */
		pool->queue[pool->tail].function = function;
		pool->queue[pool->tail].argument = argument;
		/* ���� tail �� count */
		pool->tail = next;
		pool->count += 1;

		/* pthread_cond_broadcast */
		/*
		 * ���� signal,��ʾ�� task �����ӽ�����
		 * �������Ϊ������п��������̣߳���ʱ����һ��������
		 * ���û����ʲô������
		 */
		if (pthread_cond_signal(&(pool->notify)) != 0) {
			err = threadpool_lock_failure;
			break;
		}
		/*
		 * �����õ��� do { ... } while(0) �ṹ
		 * ��֤������౻ִ��һ�Σ������м䷽����Ϊ�쳣������ִ�п�
		 */
	} while (0);

	/* �ͷŻ�������Դ */
	if (pthread_mutex_unlock(&pool->lock) != 0) {
		err = threadpool_lock_failure;
	}

	return err;
}

int threadpool_destroy(threadpool_t *pool, int flags)
{
	int i, err = 0;

	if (pool == NULL) {
		return threadpool_invalid;
	}

	/* ȡ�û�������Դ */
	if (pthread_mutex_lock(&(pool->lock)) != 0) {
		return threadpool_lock_failure;
	}

	do {
		/* Already shutting down */
		/* �ж��Ƿ����������ط��ر� */
		if (pool->shutdown) {
			err = threadpool_shutdown;
			break;
		}

		/* ��ȡָ���Ĺرշ�ʽ */
		pool->shutdown = (flags & threadpool_graceful) ?
			graceful_shutdown : immediate_shutdown;

		/* Wake up all worker threads */
		/* �������������������������̣߳����ͷŻ����� */
		if ((pthread_cond_broadcast(&(pool->notify)) != 0) ||
			(pthread_mutex_unlock(&(pool->lock)) != 0)) {
			err = threadpool_lock_failure;
			break;
		}

		/* Join all worker thread */
		/* �ȴ������߳̽��� */
		for (i = 0; i < pool->thread_count; i++) {
			if (pthread_join(pool->threads[i], NULL) != 0) {
				err = threadpool_thread_failure;
			}
		}
		/* ͬ���� do{...} while(0) �ṹ*/
	} while (0);

	/* Only if everything went well do we deallocate the pool */
	if (!err) {
		/* �ͷ��ڴ���Դ */
		threadpool_free(pool);
	}
	return err;
}

int threadpool_free(threadpool_t *pool)
{
	if (pool == NULL || pool->started > 0) {
		return -1;
	}

	/* Did we manage to allocate ? */
	/* �ͷ��߳� ������� ������ �������� �̳߳���ռ�ڴ���Դ */
	if (pool->threads) {
		free(pool->threads);
		free(pool->queue);

		/* Because we allocate pool->threads after initializing the
		   mutex and condition variable, we're sure they're
		   initialized. Let's lock the mutex just in case. */
		pthread_mutex_lock(&(pool->lock));
		pthread_mutex_destroy(&(pool->lock));
		pthread_cond_destroy(&(pool->notify));
	}
	free(pool);
	return 0;
}


static void *threadpool_thread(void *threadpool)
{
	threadpool_t *pool = (threadpool_t *)threadpool;
	threadpool_task_t task;

	for (;;) {
		/* Lock must be taken to wait on conditional variable */
		/* ȡ�û�������Դ */
		pthread_mutex_lock(&(pool->lock));

		/* Wait on condition variable, check for spurious wakeups.
		   When returning from pthread_cond_wait(), we own the lock. */
		   /* �� while ��Ϊ���ڻ���ʱ���¼������ */
		while ((pool->count == 0) && (!pool->shutdown)) {
			/* �������Ϊ�գ����̳߳�û�йر�ʱ���������� */
			pthread_cond_wait(&(pool->notify), &(pool->lock));
		}

		/* �رյĴ��� */
		if ((pool->shutdown == immediate_shutdown) ||
			((pool->shutdown == graceful_shutdown) &&
			(pool->count == 0))) {
			break;
		}

		/* Grab our task */
		/* ȡ��������еĵ�һ������ */
		task.function = pool->queue[pool->head].function;
		task.argument = pool->queue[pool->head].argument;
		/* ���� head �� count */
		pool->head += 1;
		pool->head = (pool->head == pool->queue_size) ? 0 : pool->head;
		pool->count -= 1;

		/* Unlock */
		/* �ͷŻ����� */
		pthread_mutex_unlock(&(pool->lock));

		/* Get to work */
		/* ��ʼ�������� */
		(*(task.function))(task.argument);
		/* ����һ���������н��� */
	}

	/* �߳̽����������������߳��� */
	pool->started--;

	pthread_mutex_unlock(&(pool->lock));
	pthread_exit(NULL);
	return(NULL);
}