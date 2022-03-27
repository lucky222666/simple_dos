#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#ifdef __cplusplus
/* 对于 C++ 编译器，指定用 C 的语法编译 */
extern "C" {
#endif

#define MAX_THREADS 64
#define MAX_QUEUE 65536


	typedef struct threadpool_t threadpool_t;  //线程池

	/* 定义错误码 */
	typedef enum {
		threadpool_invalid = -1,
		threadpool_lock_failure = -2,
		threadpool_queue_full = -3,
		threadpool_shutdown = -4,
		threadpool_thread_failure = -5
	} threadpool_error_t;

	typedef enum {
		threadpool_graceful = 1
	} threadpool_destroy_flags_t;

	/* 以下是线程池三个对外 API */

	//创建线程池，有 thread_count 个线程，容纳 queue_size 个的任务队列，flags 参数没有使用

	threadpool_t *threadpool_create(int thread_count, int queue_size, int flags);

	//添加任务到线程池, pool 为线程池指针，routine 为函数指针， arg 为函数参数， flags 未使用

	int threadpool_add(threadpool_t *pool, void(*routine)(void *), void *arg, int flags);


	//销毁线程池，flags 可以用来指定关闭的方式

	int threadpool_destroy(threadpool_t *pool, int flags);

#ifdef __cplusplus
}
#endif

#endif /* _THREADPOOL_H_ */