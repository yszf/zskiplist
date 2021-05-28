#pragma once

#include <iostream>
#include <stdlib.h>
#include <random>
#include <string>
#include <time.h>

#define ZSKIPLIST_MAXLEVEL 32 /* Should be enough for 2^32 elements */
#define ZSKIPLIST_P 0.25      /* Skiplist P = 1/4 */

/*
 * ��Ծ��ڵ�
 */
typedef struct zskiplistNode {

	// ��Ա����
	std::string obj;

	// ��ֵ
	double score;

	// ����ָ��
	struct zskiplistNode *backward;

	// ��
	struct zskiplistLevel {

		// ǰ��ָ��
		struct zskiplistNode *forward;

		// ���
		unsigned int span;

	} level[32];

} zskiplistNode;

/*
 * ��Ծ��
 */
typedef struct zskiplist {

	// ��ͷ�ڵ�ͱ�β�ڵ�
	struct zskiplistNode *header, *tail;

	// ���нڵ������
	unsigned long length;

	// ���в������Ľڵ�Ĳ���
	int level;

} zskiplist;

zskiplist *zslCreate();
void zslFree(zskiplist *zsl);
zskiplistNode *zslInsert(zskiplist *zsl, double score, std::string obj);
