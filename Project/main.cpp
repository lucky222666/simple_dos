#define THREAD 5
#define QUEUE  10


#include <stdio.h>
#include "pthread.h"
#include <unistd.h>
#include <assert.h>
#include<iostream>
#include "threadpool.h"
#include"Interface.h"
#include <windows.h>

#define RED 0
#define GREEN 1
#define BLUE 2
#define WHITE 3
HANDLE hdl = GetStdHandle(STD_OUTPUT_HANDLE);
using namespace std;

int tasks = 0, done = 0;
pthread_mutex_t lock;

/*
 *�ı䵱ǰ������ɫ
 */

void changeColor(int color);

void welcomeFirst();
void welcomeUser();

void dummy_task(void *arg) {
	pthread_mutex_lock(&lock);
	/* ��¼�ɹ���ɵ������� */
	done++;
	pthread_mutex_unlock(&lock);
}

int main(int argc, char **argv)
{
	welcomeFirst();
	welcomeUser();

	threadpool_t *pool;

	/* ��ʼ�������� */
	pthread_mutex_init(&lock, NULL);

	/* �����̳߳ش����ɹ� */
	assert((pool = threadpool_create(THREAD, QUEUE, 0)) != NULL);
	cout << "Pool started with " << THREAD << " threads and " << "queue size of " << QUEUE << endl;


	/* ֻҪ������л�û������һֱ��� */
	while (threadpool_add(pool, &dummy_task, NULL, 0) == 0) {
		pthread_mutex_lock(&lock);
		tasks++;
		run();
		pthread_mutex_unlock(&lock);
	}

	cout << "Added " << tasks << " tasks" << endl;


	/* ��ʱ�������̳߳�,0 ���� immediate_shutdown */
	assert(threadpool_destroy(pool, 0) == 0);
	cout << "Did " << done << " tasks" << endl;

	return 0;
}


void changeColor(int color) {
	switch (color) {
	case 0: SetConsoleTextAttribute(hdl, FOREGROUND_RED); break;
	case 1: SetConsoleTextAttribute(hdl, FOREGROUND_GREEN); break;
	case 2: SetConsoleTextAttribute(hdl, FOREGROUND_BLUE); break;
	case 3: SetConsoleTextAttribute(hdl, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); break;
	default:; break;
	}
}

void welcomeFirst() {
	changeColor(GREEN);
	printf("\n");
	printf("                    *******************************************\n");
	printf("                    *                                         *\n");
	printf("                    *                 Welcome                 *\n");
	printf("                    *                                         *\n");
	printf("                    *******************************************\n");
}

void welcomeUser() {
	changeColor(GREEN);
	printf("\n");
	printf("                    *******************************************\n");
	printf("                    *                                         *\n");
	printf("                    *       ��ӭ����༶Ŀ¼�ܹ��ļ�ϵͳ      *\n");
	printf("                    *                                         *\n");
	printf("                    *******************************************\n");
}
