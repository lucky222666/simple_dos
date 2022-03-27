#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string.h>
#include<string>
#include<iomanip>
#include <stdlib.h>
#include<stdio.h>
#include<windows.h>
#include<iomanip>
#include"FileSystem.h"

using namespace std;
extern int disk_empty = 1024 * 1024 * 1024;// ȫ�ֱ���������̿��пռ��С 1����λ����1B


string error[] = { "/","\\", ":","<",">","|","*","&" };  //�����еķǷ��ַ�

FileSystem::FileSystem() {
	size = 0;
	currentDir = NULL;
	copytempfile = NULL;
	copytempdir = NULL;
}

FileSystem::~FileSystem() {
	disk_empty += size;		//�ͷ��û���ռ�ռ�
	size = 0;				// ��0

	MyDir *d = root;
	MyFile *f = currentDir->filePtr;
	while (f != 0) {
		if (f->nextFile == 0) {
			this->dele_file(f);
			f = 0;
			break;
		}
		while (f->nextFile->nextFile != 0)
			f = f->nextFile;
		this->dele_file(f->nextFile);
		f->nextFile = 0;
		f = currentDir->filePtr;
	}
	while (d != 0) {
		if (d->nextDir == 0) {
			this->dele_dir(d);
			d = 0;
			break;
		}
		while (d->nextDir->nextDir != 0)
			d = d->nextDir;
		this->dele_dir(d->nextDir);
		d->nextDir = 0;
		d = root;
	}
}

int FileSystem::newFile() {

	MyFile *p = NULL;
	p = new MyFile;

	if (p == 0) {
		cout << "CREATE           -FALSE";
		return 0;
	}
	cout << "�����봴���ļ�������: ";
	cin >> p->name;
	/*�������*/
	string tempname(p->name);
	for (int i = 0; i < 8; ++i)
	{
		if (tempname.find(error[i], 0) != string::npos)//���ַ������±�Ϊ0����ʼ����error[i],���û�ҵ�������һ���ر�ı�־c++����npos��ʾ
		{
			cout << "RENAME            -FALSE" << endl;
			return 0;
		}
	}

	/*����ʱ���������
	 * 1. Ŀ¼��û���ļ�
	 * 2. Ŀ¼�����ļ������ļ�������ͻ
	 * 3. Ŀ¼�����ļ������ļ���������ͻ
	 * */
	 /*�������ͬ������*/
	if (currentDir->filePtr == NULL) {
		p->nextFile = nullptr;
		currentDir->filePtr = p;
	}
	else {
		MyFile *q = new MyFile;
		q = currentDir->filePtr;
		while (q != NULL) {
			if (strcmp(p->name, q->name) == 0)
			{
				cout << "FILE EXISTS             -FALSE" << endl;
				return 0;
			}
			q = q->nextFile;
		}

		/*��������ṹ,ǰ�巨*/
		p->nextFile = currentDir->filePtr;
		p->size = 0;
		currentDir->filePtr = p;
		MyDir *h = currentDir;

		/*�����ϼ�Ŀ¼�Ĵ�С*/
		while (h != NULL) {
			h->size += p->size;
			h = h->preDir;
		}

	}
	currentDir->filePtr->size = 0;
	cout << "CREATE             -OK" << endl;
	disk_empty = disk_empty - p->size;
	size += p->size;
	return 1;
}

int FileSystem::newDir() {
	MyDir *p, *h;
	p = new MyDir;
	cout << "�����봴��Ŀ¼������: ";
	cin >> p->name;

	/*�������*/
	string tempname(p->name);
	for (int i = 0; i < 8; ++i)
	{
		if (tempname.find(error[i], 0) != string::npos)
		{
			cout << "RENAME              -FALSE" << endl;
			return 0;
		}
	}


	p->dirPtr = NULL;
	p->size = 0;
	p->filePtr = NULL;
	p->nextDir = NULL;
	if (currentDir->dirPtr == NULL)
		h = NULL;
	else
		h = currentDir->dirPtr;

	/*����ʱ���������
	 * 1. Ŀ¼��û����Ŀ¼
	 * 2. Ŀ¼������Ŀ¼��������ͻ
	 * 3. Ŀ¼������Ŀ¼����������ͻ
	 * */
	 /*�������ͬ��Ŀ¼*/

	while (h != NULL) {
		if (strcmp(h->name, p->name) == 0) {
			cout << "DIR EXISTS           -FALSE" << endl;
			return 0;
		}
		h = h->nextDir;
	}

	/*��������ṹ*/
	p->preDir = currentDir;
	p->nextDir = currentDir->dirPtr;
	currentDir->dirPtr = p;

	cout << "CREATE                -OK" << endl;
	return 1;
}

int FileSystem::openFile() {
	char n[MAX_NAME];

	cout << "�����뵱ǰ·���½�Ҫ���ļ�������: ";
	cin >> n;
	MyFile *f = currentDir->filePtr;
	while (f != 0) {
		if (strcmp(f->name, n) == 0) {
			cout << "Successfully opened!" << endl;
			return 1;	   //1�ɹ��� 
		}
		f = f->nextFile;
	}
	cout << "NO FILE              -FALSE" << endl;
	return 0;			  //0��ʧ��
}

int FileSystem::closeFile() {
	char n[MAX_NAME];

	cout << "�����뵱ǰ·���½�Ҫ�ر��ļ�������: ";
	cin >> n;
	MyFile *f = currentDir->filePtr;
	while (f != 0) {
		if (strcmp(f->name, n) == 0) {
			cout << "Successfully closed!" << endl;
			return 1;				 //1�رճɹ�   
		}
		f = f->nextFile;
	}
	cout << "NO FILE              -FALSE" << endl;
	return 0;						//0�ر�ʧ��
}

int FileSystem::dele_file(MyFile *f) {
	delete f;
	f = NULL;
	return 1;
}

int FileSystem::deleteFile() {
	char temp[MAX_NAME];

	cout << "������ɾ���ļ�������: ";
	cin >> temp;
	MyFile *f;
	MyFile *above = NULL;
	f = currentDir->filePtr;

	/*
	 * �жϸ�Ŀ¼��������Ҫɾ�����ļ�
	 * */

	while (f != NULL) {
		if (!strcmp(f->name, temp))
			break;
		above = f;
		f = f->nextFile;
	}
	if (f == NULL) {
		cout << "NO FILE              -FALSE" << endl;
		return 0;
	}
	disk_empty += f->size;
	MyDir *d = currentDir;
	while (d != 0) //�޸�ɾ���ļ������Ŀ¼�Ĵ�С
	{
		d->size -= f->size;
		d = d->preDir;
	}

	/*
	 * ɾ��ʱ����
	 * 1. ��Ҫɾ�����ļ�ǡ����Ŀ¼�ļ������е�ͷ�ڵ�
	 * 2. ��Ҫɾ�����ļ��������м�
	 * */

	if (f == currentDir->filePtr)//ɾ���ļ����
		currentDir->filePtr = currentDir->filePtr->nextFile;
	else
		above->nextFile = f->nextFile;
	size -= f->size;
	delete f;
	f = NULL;
	cout << "DELETE             -OK" << endl;
	return 1;
}

int FileSystem::dele_dir(MyDir *d) {
	delete d;
	d = NULL;
	return 1;
}

int FileSystem::deleteDir() {
	char n[MAX_NAME];

	MyDir *p, *pre = NULL;
	p = root;
	p = currentDir->dirPtr;

	cout << "������ɾ��Ŀ¼������: ";
	cin >> n; //ɾ�����ļ���

	/*��������Ҫɾ����Ŀ¼*/
	while (p != NULL) {
		if (strcmp(p->name, n) == 0)
		{
			pre = p; break;
		}
		p = p->nextDir;
	}

	if (p == NULL) {
		cout << "DELETE            -FALSE" << endl;
		return 0;
	}

	/*ɾ��Ŀ¼ʱ��Ҫ����
	 * 1. ��Ŀ¼���ڸ�Ŀ¼��Ŀ¼�����λ��
	 * 2. ��Ŀ¼���Ƿ�����Ŀ¼�������ļ�
	 * */
	disk_empty += p->size;
	if (p == currentDir->dirPtr)
		currentDir->dirPtr = currentDir->dirPtr->nextDir;
	else
		p->preDir->nextDir = p->nextDir;

	pre = currentDir;
	while (pre != NULL) //�޸�ɾ��Ŀ¼�����Ŀ¼��С
	{
		pre->size -= p->size;
		pre = pre->preDir;
	}
	size -= p->size;
	MyDir *d = p->dirPtr;
	MyFile *f = p->filePtr;
	if (f != 0) {
		while (p->filePtr->nextFile != NULL)//ɾ����Ŀ¼�µ��ļ�
		{
			f = p->filePtr;
			while (f->nextFile->nextFile != NULL)//Ѱ�����һ���ļ����
				f = f->nextFile;
			this->dele_file(f->nextFile);
			f->nextFile = NULL;
		}
		if (p->filePtr->nextFile == NULL) {
			this->dele_file(p->filePtr);
			p->filePtr = NULL;
		}
	}
	if (d != NULL) {
		while (p->dirPtr->nextDir != NULL)//ɾ����Ŀ¼�µ�Ŀ¼
		{
			d = p->dirPtr;
			while (d->nextDir->nextDir != NULL)//Ѱ�����һ���ļ����
				d = d->nextDir;
			this->dele_dir(d->nextDir);//�ݹ���ô˺���
			d->nextDir = NULL;
		}
		if (p->dirPtr->nextDir == NULL) {
			this->dele_dir(p->dirPtr);
			p->dirPtr = NULL;
		}
	}
	delete p, d, f;

	cout << "DELETE           -OK" << endl;
	return 1;

}

int FileSystem::readDir() {
	char name[MAX_NAME];
	MyDir *p;
	p = currentDir->dirPtr;

	cout << "�������ȡĿ¼������: ";
	cin >> name;
	while (p != NULL) {
		if (strcmp(p->name, name) == 0) {
			currentDir = p;
			return 1;
		}
		p = p->nextDir;
	}
	cout << "NO DIR             -FALSE" << endl;
	return 0;
}

string FileSystem::readFile() {
	char n[MAX_NAME];
	string text;

	cout << "�������ȡ�ļ�������: ";
	cin >> n;
	MyFile *f = currentDir->filePtr;
	while (f != 0) {
		if (strcmp(f->name, n) == 0) {
			cout << f->content << endl;
			text = f->content;
			return "None";
		}
		f = f->nextFile;
	}
	cout << "NO FILE              -FALSE" << endl;
	return text;
}

int FileSystem::renameDir() {
	MyDir *h;
	char n1[MAX_NAME], n2[MAX_NAME];

	cout << "�����뽫����Ŀ¼�ľ�����: ";
	cin >> n1;
	cout << "�����뽫����Ŀ¼��������: ";
	cin >> n2;

	h = currentDir->dirPtr;
	while (h != NULL) {
		if (strcmp(h->name, n2) == 0) {
			cout << "DIR EXIST        -FALSE" << endl;
			return 0;
		}
		h = h->nextDir;
	}
	h = currentDir->dirPtr;
	while (h != NULL) {
		if (strcmp(h->name, n1) == 0)
			break;
		h = h->nextDir;
	}
	if (h == NULL) {
		cout << "NO DIR         -FALSE" << endl;
		return 0;
	}
	strcpy(h->name, n2);
	cout << "RENAME            -OK" << endl;
	return 1;
}

int FileSystem::renameFile() {
	MyFile *h;
	char n1[MAX_NAME], n2[MAX_NAME];

	cout << "�����뽫�����ļ��ľ�����: ";
	cin >> n1;
	cout << "�����뽫�����ļ���������: ";
	cin >> n2;

	h = currentDir->filePtr;
	while (h != NULL) {
		if (!strcmp(h->name, n2)) {
			cout << "FILE EXISTS           -FALSE" << endl;
			return 0;
		}
		h = h->nextFile;
	}
	h = currentDir->filePtr;
	while (h != NULL) {
		if (!strcmp(h->name, n1))
			break;
		h = h->nextFile;
	}
	if (h == NULL) {
		cout << "NO FILE            -FALSE" << endl;
		return 0;
	}
	strcpy(h->name, n2);
	cout << "RENAME             -OK" << endl;
	return 1;
}

MyDir *FileSystem::copy_dir(MyDir *d) {
	MyFile *fh;
	MyDir *dh, *dir;
	dir = new MyDir;
	dir->dirPtr = NULL;
	dir->filePtr = NULL;
	dir->nextDir = NULL;
	dir->preDir = NULL;
	fh = d->filePtr;
	dh = d->dirPtr;

	while (fh != NULL) {
		MyFile *f;
		f = copy_file(fh);
		f->nextFile = dir->filePtr;
		dir->filePtr = f;
		fh = fh->nextFile;
	}

	while (dh != NULL) {
		MyDir *d1, *d2;
		d1 = copy_dir(dh);
		d1->preDir = dir;
		dir->dirPtr = d1;
		d2 = dh->nextDir;
		while (d2 != NULL) {
			d1->nextDir = copy_dir(d2);

			d2 = d2->nextDir;
			d1 = d1->nextDir;
			d1->preDir = dir;
		}

		dh = dh->dirPtr;
	}

	strcpy(dir->name, d->name);
	dir->size = d->size;
	return dir;
}

MyDir *FileSystem::copyDir() {
	MyDir *h;
	char n[MAX_NAME];

	cout << "�����뵱ǰ·���¼ƻ�����/����/����Ŀ¼������: ";
	cin >> n;
	h = currentDir->dirPtr;
	while (h != NULL) {
		if (!strcmp(h->name, n))
			break;
		h = h->nextDir;
	}
	if (h == NULL) {
		cout << "NO DIR            -FALSE" << endl;
		return 0;
	}
	copytempdir = copy_dir(h);
	cout << "COPY           -OK" << endl;
	return copytempdir;
}

MyFile *FileSystem::copy_file(MyFile *h) {
	MyFile *f;
	f = new MyFile;
	f->size = h->size;
	strcpy(f->name, h->name);
	f->content = h->content;
	return f;
}

MyFile *FileSystem::copyFile() {
	MyFile *h;
	char n[MAX_NAME];

	cout << "�����뵱ǰ·���¼ƻ������ļ�������: ";
	cin >> n;
	h = currentDir->filePtr;

	while (h != NULL) {
		if (!strcmp(h->name, n))
			break;
		h = h->nextFile;
	}
	if (h == NULL) {
		cout << "NO FILE             -FALSE" << endl;
		return NULL;
	}
	copytempfile = copy_file(h);
	cout << "COPY             -OK" << endl;
	return copytempfile;
}

int FileSystem::pasteDir() {	//���ƻ�����Ŀ¼���Ƶ���ǰ·���µ�Ŀ¼������
	MyDir *h, *d1;
	h = currentDir->dirPtr;

	if (copytempdir == NULL) {
		cout << "NO SOURCE            -FALSE" << endl;
		return 0;
	}
	if (h == NULL) {
		if (disk_empty < copytempdir->size) {
			cout << "NO ENOUGH SPACE             -FALSE";
			return 0;
		}
		disk_empty -= copytempdir->size;
		d1 = copy_dir(copytempdir);
		currentDir->dirPtr = d1;
		d1->preDir = currentDir;
		currentDir->size += copytempdir->size;

	}
	else {		 //h!= NULL
		while (h != NULL) {
			if (!strcmp(h->name, copytempdir->name))
				break;
			h = h->nextDir;
		}
		if (h != NULL) {
			cout << "DIR EXISTS          -FALSE" << endl;
			return 0;
		}
		else {
			if (disk_empty < copytempdir->size) {
				cout << "NO ENOUGH SPACE            -FALSE";
				return 0;
			}
			d1 = copy_dir(copytempdir);
			d1->nextDir = currentDir->dirPtr;
			currentDir->dirPtr = d1;
			d1->preDir = currentDir;
			currentDir->size += copytempdir->size;
		}
	}
	cout << "PASTE              -OK" << endl;
	return 1;
}

int FileSystem::pasteFile() {
	MyFile *h = currentDir->filePtr;
	MyFile *pTemp = h;
	if (copytempfile == NULL) {
		cout << "NO SOURCE              -FALSE" << endl;
		return 0;
	}
	/*�����ǰĿ¼û���ļ�*/
	if (h == NULL) {
		if (disk_empty < copytempfile->size) {
			cout << "NO ENOUGH SPACE            -FALSE";
			return 0;
		}
		currentDir->filePtr = copy_file(copytempfile);
		currentDir->size += copytempfile->size;
	}
	else {
		while (h != NULL) {
			if (!strcmp(h->name, copytempfile->name)) {
				cout << "FILE EXISTS                -FALSE" << endl;
				return 0;
			}
			//break;
			h = h->nextFile;
		}

		if (disk_empty < copytempfile->size) {
			cout << "NO ENOUGH SPACE              -FALSE";
			return 0;
		}
		currentDir->filePtr = copy_file(copytempfile);
		currentDir->filePtr->nextFile = pTemp;
		//currentDir->filePtr->nextFile = h;
		currentDir->size += copytempfile->size;
		cout << "PASTE                -OK" << endl;
		return 1;
	}
	return 0;

}

//д�ļ�ת������,������ĺ���ʽһ���
void Convert(string &buf)
{
	//ʹ��$��Ϊ�ո� #��Ϊ���з�
	char Space = 32;	 //" "
	char Endter = '\n';

	int len = buf.length();
	for (int i = 0; i < len; i++)	 //ת������
	{
		if (buf[i] == '$')
			buf[i] = Space;
		else if (buf[i] == '#')
			buf[i] = Endter;
	}
}

int FileSystem::edit(int flag, string &text) {
	char n[MAX_NAME];
	string s;
	if (flag == 1) {
		cout << "������༭�ļ�������: ";
		cin >> n;
		MyFile *f = currentDir->filePtr;
		while (f != 0) {
			if (!strcmp(f->name, n))
			{
				cout << "��༭����(ʹ��$��Ϊ�ո� #��Ϊ���з�): " << endl;
				cin >> s;
				Convert(s);//��������ı���һ������
				f->content += s;
				f->size = s.length();
				disk_empty -= f->size;
				MyDir *d = currentDir;
				while (d != 0)//�޸ı༭�ļ������Ŀ¼�Ĵ�С
				{
					d->size += f->size;
					d = d->preDir;
				}
				cout << "EDIT              -OK" << endl;
				size += f->size;
				return 1;
			}
			f = f->nextFile;
		}

		cout << "NO FILE                -FALSE" << endl;
	}
	else				  //flag==0
	{
		cout << "������༭�ļ�������: ";
		cin >> n;
		MyFile *f = currentDir->filePtr;
		while (f != 0) {
			if (!strcmp(f->name, n))
			{
				s = text;
				Convert(s);//��������ı���һ������
				f->content += s;
				f->size = s.length();
				disk_empty -= f->size;
				MyDir *d = currentDir;
				while (d != 0)//�޸ı༭�ļ������Ŀ¼�Ĵ�С
				{
					d->size += f->size;
					d = d->preDir;
				}
				cout << "EDIT              -OK" << endl;
				size += f->size;
				/*cout << "��������ļ��洢�ľ���·��(�ǵö�Ӧ��ϵ): ";	 //��G:\\OS_CD\\file1.txt...��
				cin >> n;
				FILE *fp = fopen(n, "a+");
				fwrite(&s, sizeof(s), 1, fp);
				fclose(fp);	  */
				return 1;
			}
			f = f->nextFile;
		}

		cout << "NO FILE                -FALSE" << endl;
	}
	return 0;
}

int FileSystem::show_path(MyDir *d)//ʵ����ʾ��ǰ·���ĺ���
{
	if (d->preDir == NULL)	 //˵����ǰ��root��
		cout << root->name;
	if (d->preDir != NULL) {
		this->show_path(d->preDir);//�ݹ���ô˺���
		cout << d->name;
	}
	cout << "/";
	return 1;
}

int FileSystem::showCurrentDir() {
	MyDir *d = currentDir->dirPtr;
	MyFile *f = currentDir->filePtr;
	if (d == NULL && f == NULL) {
		cout << "EMPTY" << endl;
		return 0;
	}
	cout << "CONTENT-Tree:" << endl;

	if (d != NULL) {
		cout << "\t" << "D_NAME" << "\t" << "D_SIZE" << endl;
		while (d != NULL) {
			cout << "\t" << d->name << "\t" << d->size << endl;
			d = d->nextDir;
		}
		cout << endl;
	}

	if (f != NULL) {
		cout << "\t" << "F_NAME" << "\t" << "F_SIZE" << "\t" << endl;
		while (f != nullptr) {
			cout << "\t" << f->name << "\t" << f->size << "\t" << endl;
			f = f->nextFile;
		}
	}
	return 1;
}

int FileSystem::showPath()//��ʾ��ǰ·��
{
	show_path(currentDir);
	return 1;
}

int FileSystem::goback() {
	if (currentDir->preDir == NULL) {	//�Ѿ���root��
		cout << "ROOT DIR          -FALSE" << endl;
		return 0;
	}
	currentDir = currentDir->preDir;
	return 1;
}

int FileSystem::findFile(MyDir *dir, char *name) {	  //�ݹ��ѯ
	MyDir *pre;
	MyFile *p;

	p = currentDir->filePtr;
	pre = currentDir->dirPtr;
	//��ѯ�ļ�����
	cout << "��ѯ�ļ����� " << endl;
	while (p != NULL) {
		if (strcmp(p->name, name) == 0) {
			cout << "��ѯ�ļ��ɹ������ڴ�Ŀ¼�£�" << endl;
			return 1;
		}
		p = p->nextFile;
	}
	cout << "NO FILE             -FALSE" << endl;
	//��ѯĿ¼����
	cout << "��һ���ڸ�����Ŀ¼�²�ѯ " << endl;
	while (pre != NULL)
	{
		cout << "��ǰ����Ŀ¼��: " << pre->name << endl;
		findFile(pre, name);
		//���ݹ���Ŀ¼�ĺ����ó�nextDir��ϵ����
		pre = pre->nextDir;
	}
	cout << "FINALLY,NO FILE     -FALSE" << endl;
	return 0;
}

int FileSystem::findFiles()
{
	char n[MAX_NAME];
	int cono = 0, len = 0;//����������
	cout << "������ҪѰ�ҵ��ļ��࣬��ʽ����Ҫ������*.txt *.docx: ";
	cin >> n;
	while (n[len] != '\0')
		len++;
	MyFile *p = currentDir->filePtr;
	while (p != NULL)
	{
		string tempname(p->name);
		for (int i = 0; i < 8; ++i)
		{
			if (tempname.find(n[i], 0) != string::npos)//���ַ������±�Ϊ0����ʼ����error[i],���û�ҵ�������һ���ر�ı�־c++����npos��ʾ
			{
				cono++;										//��.��ʼƥ��
				if (cono == len - 1)
				{
					cout << tempname << "\t"; break;
				}

			}
			else cono = 0;
		}
		p = p->nextFile;
	}
	cout << endl;
	return 0;
}

int FileSystem::exportFile(string &text)
{
	char path[200];
	cout << "������ƻ������ļ���Ŀ�ĵ�: ";
	cin >> path;
	FILE *fp = fopen(path, "a+");		//�����ݵ�����ָ��λ�ã���ָ̬����desktop
	fwrite(&text, sizeof(text), 1, fp);
	fclose(fp);

	return 0;
}

char*  FileSystem::importFile()
{
	FILE *fp;
	char path[200];
	cout << "������ƻ������ļ�����Դ��: ";
	cin >> path;
	fp = fopen(path, "rt");				  //"G:\OS_CD\file1.txt"
	//����ļ��Ĵ�С
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	cout << "��������ļ���С�� " << size << " B" << endl;
	rewind(fp);

	//����һ����װ�������ļ��Ŀռ� 
	char *ar = (char *)malloc(sizeof(char)*MAX_NAME);
	//���ļ���ÿ�ζ�һ��������size�� 
	fread(ar, sizeof(ar), 1, fp);
	fclose(fp);

	return ar;
}

int FileSystem::setUser(char *n, char *c) {
	MyDir *root = new MyDir;
	strcpy(root->name, n);
	strcpy(name, n);
	strcpy(password, c);

	this->root = root;
	currentDir = root;
	currentDir->preDir = NULL;
	currentDir->dirPtr = NULL;
	currentDir->filePtr = NULL;
	currentDir->nextDir = NULL;
	return 1;
}

int FileSystem::getSize() {
	return size;
}


MyDir *FileSystem::getCurrentdir() {
	return currentDir;
}

char *FileSystem::getRootName() {
	return name;
}
int FileSystem::getVersion() {
	cout << "Virtual FileSys [�汾 2020.05.28.2]" << endl;
	return 0;
}
