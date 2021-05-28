#pragma once

#include <iostream>
#include <stdlib.h>
#include <random>
#include <string>
#include <time.h>

#define ZSKIPLIST_MAXLEVEL 32 /* Should be enough for 2^32 elements */
#define ZSKIPLIST_P 0.25      /* Skiplist P = 1/4 */

/*
 * 跳跃表节点
 */
typedef struct zskiplistNode {

	// 成员对象
	std::string obj;

	// 分值
	double score;

	// 后退指针
	struct zskiplistNode *backward;

	// 层
	struct zskiplistLevel {

		// 前进指针
		struct zskiplistNode *forward;

		// 跨度
		unsigned int span;

	} level[32];

} zskiplistNode;

/*
 * 跳跃表
 */
typedef struct zskiplist {

	// 表头节点和表尾节点
	struct zskiplistNode *header, *tail;

	// 表中节点的数量
	unsigned long length;

	// 表中层数最大的节点的层数
	int level;

} zskiplist;

zskiplist *zslCreate();
void zslFree(zskiplist *zsl);
zskiplistNode *zslInsert(zskiplist *zsl, double score, std::string obj);
