#include "zskiplist.h"

/*
 * ����һ������Ϊ level ����Ծ��ڵ㣬
 * �����ڵ�ĳ�Ա��������Ϊ obj ����ֵ����Ϊ score ��
 *
 * ����ֵΪ�´�������Ծ��ڵ�
 *
 * T = O(1)
 */
zskiplistNode *zslCreateNode(int level, double score, std::string obj) {

	// ����ռ�
	zskiplistNode *zn = new zskiplistNode;

	// ��������
	zn->score = score;
	zn->obj = obj;

	return zn;
}

/*
 * ����������һ���µ���Ծ��
 *
 * T = O(1)
 */
zskiplist *zslCreate(void) {
	int j;
	zskiplist *zsl;

	// ����ռ�
	zsl = new zskiplist;

	// ���ø߶Ⱥ���ʼ����
	zsl->level = 1;
	zsl->length = 0;

	// ��ʼ����ͷ�ڵ�
	// T = O(1)
	zsl->header = zslCreateNode(ZSKIPLIST_MAXLEVEL, 0, "");
	for (j = 0; j < ZSKIPLIST_MAXLEVEL; j++) {
		zsl->header->level[j].forward = NULL;
		zsl->header->level[j].span = 0;
	}
	zsl->header->backward = NULL;

	// ���ñ�β
	zsl->tail = NULL;

	return zsl;
}

/*
 * �ͷŸ�������Ծ��ڵ�
 *
 * T = O(1)
 */
void zslFreeNode(zskiplistNode *node) {
	delete node;
}

/*
 * �ͷŸ�����Ծ���Լ����е����нڵ�
 *
 * T = O(N)
 */
void zslFree(zskiplist *zsl) {

	zskiplistNode *node = zsl->header->level[0].forward, *next;

	// �ͷű�ͷ
	delete zsl->header;

	// �ͷű������нڵ�
	// T = O(N)
	while (node) {

		next = node->level[0].forward;

		zslFreeNode(node);

		node = next;
	}

	// �ͷ���Ծ��ṹ
	delete zsl;
}

/* Returns a random level for the new skiplist node we are going to create.
 *
 * ����һ�����ֵ����������Ծ��ڵ�Ĳ�����
 *
 * The return value of this function is between 1 and ZSKIPLIST_MAXLEVEL
 * (both inclusive), with a powerlaw-alike distribution where higher
 * levels are less likely to be returned.
 *
 * ����ֵ��� 1 �� ZSKIPLIST_MAXLEVEL ֮�䣨���� ZSKIPLIST_MAXLEVEL����
 * ��������㷨��ʹ�õ��ݴζ��ɣ�Խ���ֵ���ɵļ���ԽС��
 *
 * T = O(N)
 */
int zslRandomLevel(void) {
	int level = 1;

	while ((rand() & 0xFFFF) < (ZSKIPLIST_P * 0xFFFF))
		level += 1;

	return (level < ZSKIPLIST_MAXLEVEL) ? level : ZSKIPLIST_MAXLEVEL;
}

/*
 * ����һ����ԱΪ obj ����ֵΪ score ���½ڵ㣬
 * ��������½ڵ���뵽��Ծ�� zsl �С�
 *
 * �����ķ���ֵΪ�½ڵ㡣
 *
 * T_wrost = O(N^2), T_avg = O(N log N)
 */
zskiplistNode *zslInsert(zskiplist *zsl, double score, std::string obj) {
	zskiplistNode *update[ZSKIPLIST_MAXLEVEL], *x;
	unsigned int rank[ZSKIPLIST_MAXLEVEL];
	int i, level;

	//redisAssert(!isnan(score));

	// �ڸ�������ҽڵ�Ĳ���λ��
	// T_wrost = O(N^2), T_avg = O(N log N)
	x = zsl->header;
	for (i = zsl->level - 1; i >= 0; i--) {

		/* store rank that is crossed to reach the insert position */
		// ��� i ���� zsl->level-1 ��
		// ��ô i �����ʼ rank ֵΪ i+1 ��� rank ֵ
		// ������� rank ֵһ����ۻ�
		// ���� rank[0] ��ֵ��һ�����½ڵ��ǰ�ýڵ����λ
		// rank[0] ���ں����Ϊ���� span ֵ�� rank ֵ�Ļ���
		rank[i] = i == (zsl->level - 1) ? 0 : rank[i + 1];

		// ����ǰ��ָ�������Ծ��
		// T_wrost = O(N^2), T_avg = O(N log N)
		while (x->level[i].forward &&
			(x->level[i].forward->score < score ||
				// �ȶԷ�ֵ
			(x->level[i].forward->score == score &&
				// �ȶԳ�Ա�� T = O(N)
				x->level[i].forward->obj < obj))) {

			// ��¼��;��Խ�˶��ٸ��ڵ�
			rank[i] += x->level[i].span;

			// �ƶ�����һָ��
			x = x->level[i].forward;
		}
		// ��¼��Ҫ���½ڵ������ӵĽڵ�
		update[i] = x;
	}

	/* we assume the key is not already inside, since we allow duplicated
	 * scores, and the re-insertion of score and redis object should never
	 * happen since the caller of zslInsert() should test in the hash table
	 * if the element is already inside or not.
	 *
	 * zslInsert() �ĵ����߻�ȷ��ͬ��ֵ��ͬ��Ա��Ԫ�ز�����֣�
	 * �������ﲻ��Ҫ��һ�����м�飬����ֱ�Ӵ�����Ԫ�ء�
	 */

	 // ��ȡһ�����ֵ��Ϊ�½ڵ�Ĳ���
	 // T = O(N)
	level = zslRandomLevel();

	// ����½ڵ�Ĳ����ȱ��������ڵ�Ĳ�����Ҫ��
	// ��ô��ʼ����ͷ�ڵ���δʹ�õĲ㣬�������Ǽ�¼�� update ������
	// ����Ҳָ���½ڵ�
	if (level > zsl->level) {

		// ��ʼ��δʹ�ò�
		// T = O(1)
		for (i = zsl->level; i < level; i++) {
			rank[i] = 0;
			update[i] = zsl->header;
			update[i]->level[i].span = zsl->length;
		}

		// ���±��нڵ�������
		zsl->level = level;
	}

	// �����½ڵ�
	x = zslCreateNode(level, score, obj);

	// ��ǰ���¼��ָ��ָ���½ڵ㣬������Ӧ������
	// T = O(1)
	for (i = 0; i < level; i++) {

		// �����½ڵ�� forward ָ��
		x->level[i].forward = update[i]->level[i].forward;

		// ����;��¼�ĸ����ڵ�� forward ָ��ָ���½ڵ�
		update[i]->level[i].forward = x;

		/* update span covered by update[i] as x is inserted here */
		// �����½ڵ��Խ�Ľڵ�����
		x->level[i].span = update[i]->level[i].span - (rank[0] - rank[i]);

		// �����½ڵ����֮����;�ڵ�� span ֵ
		// ���е� +1 ��������½ڵ�
		update[i]->level[i].span = (rank[0] - rank[i]) + 1;
	}

	/* increment span for untouched levels */
	// δ�Ӵ��Ľڵ�� span ֵҲ��Ҫ��һ����Щ�ڵ�ֱ�Ӵӱ�ͷָ���½ڵ�
	// T = O(1)
	for (i = level; i < zsl->level; i++) {
		update[i]->level[i].span++;
	}

	// �����½ڵ�ĺ���ָ��
	x->backward = (update[0] == zsl->header) ? NULL : update[0];
	if (x->level[0].forward)
		x->level[0].forward->backward = x;
	else
		zsl->tail = x;

	// ��Ծ��Ľڵ������һ
	zsl->length++;

	return x;
}