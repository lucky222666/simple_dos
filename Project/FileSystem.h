#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include<iostream>
#include<string.h>
#include<string>
#include<iomanip>
#include<stdlib.h>
#include<stdio.h>
using namespace std;
#define MAX_NAME  20     //�ļ�������Ŀ¼��������ֽ���
extern int disk_empty;	 //�����洢�Ĵ��̴�С


typedef struct MyFile         //�ļ��ṹ��
{
	char name[MAX_NAME];      //�ļ��� 
	int size;                 //�ļ���С
	struct MyFile *nextFile;  //ָ���ļ��б�����һ���ļ�
	string content;           //�ļ�����
} MyFile;

typedef struct MyDir          //Ŀ¼�ṹ��
{
	char name[MAX_NAME];      //Ŀ¼����
	int size;                 //Ŀ¼��С
	MyDir *nextDir;           //���Ŀ¼
	MyDir *preDir;            //ǰ��Ŀ¼
	MyFile *filePtr;          //��Ŀ¼�µ��ļ�����ָ��
	MyDir *dirPtr;            //��Ŀ¼�µ�Ŀ¼����ָ��
} MyDir;


class FileSystem //�ļ�ϵͳ��
{
private:
	MyDir *currentDir;			//��ǰĿ¼
	MyFile *copytempfile;	    //���ڿ����ļ�ʱ����ʱ�ļ�
	MyDir *copytempdir;			//���ڿ���Ŀ¼ʱ����ʱĿ¼
	MyDir *root;				//��Ŀ¼
	char password[MAX_NAME];    //�û�����
	char name[MAX_NAME];		//�û�����
	int size;					//�û���ʹ�ÿռ��С

public:
	FileSystem();
	~FileSystem();

	int newFile();				 //�����ļ�
	int newDir();				 //����Ŀ¼

	int openFile();				 //���ļ�
	int closeFile();			 //�ر��ļ�

	int dele_file(MyFile *file); //ɾ���ļ�
	int deleteFile();            //ɾ���ļ�ǰ���߼��ж�
	int dele_dir(MyDir *d);      //ɾ��Ŀ¼
	int deleteDir();			 //ɾ��Ŀ¼ǰ���߼��ж�

	int readDir();				//��Ŀ¼
	string readFile();			//���ļ�

	int renameDir();			//������Ŀ¼
	int renameFile();			//�������ļ�

	MyDir *copy_dir(MyDir *dir);	//����Ŀ¼
	MyDir *copyDir();				//����Ŀ¼ǰ���߼��ж�
	MyFile *copy_file(MyFile *h);	//�����ļ�
	MyFile *copyFile();				//�����ļ�ǰ���߼��ж�

	int pasteDir();					//ճ��Ŀ¼
	int pasteFile();				//ճ���ļ�
	int edit(int flag, string &);	//�༭�ļ�

	int showCurrentDir();			//��ʾ��ǰĿ¼����

	int show_path(MyDir *dir);		//��ʾ·���Ĳ���ʵ��
	int showPath();					//��ʾ·��

	int findFile(MyDir *dir, char *name);	//�����ļ�
	int findFiles();						//������ͬ��׺�ļ�

	int   exportFile(string &text);			//�����ļ���ָ��λ��
	char* importFile();						//�����ļ���ָ��Ŀ¼�µ��ļ���

	int goback();							//������һ��Ŀ¼

	int setUser(char *, char *);			//�����û���������

	int getSize();							//����û�����Ŀ¼��С

	MyDir *getCurrentdir();					//��õ�ǰĿ¼

	char *getRootName();					//��ȡ��Ŀ¼����

	int getVersion();						//��ȡ�ļ�ϵͳ�汾��Ϣ
};

