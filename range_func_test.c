// range_test1.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <assert.h>

typedef unsigned int        _u32;
typedef int _i32;

typedef struct  _tag_range
{
    _u32  _index;
    _u32   _num;
} RANGE;

typedef struct _range_list_node
{
    RANGE   _range;
    struct _range_list_node* _next_node;
    struct _range_list_node* _prev_node;
} RANGE_LIST_NODE ,* PRANGE_LIST_NODE;

typedef struct _tag_range_list
{
    _u32 _node_size;
    RANGE_LIST_NODE* _head_node;
    RANGE_LIST_NODE* _tail_node;
} RANGE_LIST;

/*
	�������ܣ�
		�Ƚ�����range�Ĵ�С������ıȽϷ����μ�����ֵ��Ϣ������
	����ֵ��
		lval��rval��ȣ�����0��
		���߽�����lval��rval�ұߣ�����1��
		���߽�����lval��rval��ߣ�����-1��
		lval��rval����������2��
		rval��lval����������-2��
		�����޽�����lval���ұߣ�����3��
		�����޽�����rval���ұߣ�����-3��
		�������󷵻�-4
***/
_i32 range_complete(const RANGE *lval, const RANGE *rval)
{
    _u32 lval_begin;
    _u32 rval_begin;
    _u32 lval_end;
    _u32 rval_end;

	//������Ч��
	if (lval == NULL || rval == NULL)
	{
		return -4;
	}
	lval_begin = lval->_index;
	rval_begin = rval->_index;
	lval_end = lval->_index + lval->_num;
	rval_end = rval->_index + rval->_num;

    if ((lval_begin == rval_begin) && (lval_end == rval_end))
    {
        return 0;
    }

    if (lval_end <= rval_begin)
    {
        return -3;
    }
    else if (lval_end <= rval_end)
    {
        if (lval_begin <= rval_begin)
        {
            return -1;
        }
        else
        {
            return 2;
        }
    }
    else
    {
        if (lval_begin <= rval_begin)
        {
            return -2;
        }
        else if (lval_begin >= rval_end)
        {
            return 3;
        }
        else
        {
            return 1;
        }
    }
}

/*
	�������ܣ�
		����range_list��range���ж��Ƿ���β�ص���
	����ֵ��
		 -1 ��ʾ��������
		 0  ��ʾr ��range_list ��һ����ͷ���ص�
		 1  ��ʾr ��range_list ��һ����β���ص�
		 2  ��ʾr ��range_list ���ص�
		 ע����r��range_list��β���ص����������ж�ͷ���ص���
*/
_i32 range_list_is_head_relevant(RANGE_LIST* range_list, const RANGE* r)
{
    RANGE_LIST_NODE *cur_node = NULL;
    int complete_res;
    // ������Ч��
    if(range_list == NULL || r == NULL || r->_num == 0)
        return  -1;

    cur_node = range_list->_head_node;
    while (cur_node != NULL)
    {
        complete_res = range_complete(&cur_node->_range, r);
        switch (complete_res)
        {
            case -3:
                cur_node = cur_node->_next_node;
                break;
            case -2:
            case -1:
            case 0:
                return 0;
            case 1:
            case 2:
                return 1;

                //case 3:
            default:
                return 2;
        }
    }
    return 2;
}
int test_range_complete()
{
    RANGE lVal[7] = {{1, 1}, {6, 1},{1, 4},{3, 1},{1, 2},{3, 2},{2, 2}};
    RANGE rVal[7] = {{2, 2}, {2, 2},{2, 2},{2, 2},{2, 2},{2, 2},{2, 2}};
    size_t i = 0;
    enum { TEST_CASE_NUM = sizeof(lVal) / sizeof(RANGE)};

	printf("%s run.\n", __FUNCTION__);
    for (i = 0; i < 7; i ++)
    {
        printf("test[%i]: result = %i\n", i+1,  range_complete(&lVal[i], &rVal[i]));
	}
	printf("test[%i]: result = %i\n", i+1,  range_complete(NULL, NULL));

    return 0;
}

int test_range_list_is_head_relevant()
{
    RANGE ranges[] = {{0, 1}, {0, 5}, {2, 4}, {1, 5}, {3, 4}, {6, 1}};
    //RANGE ranges2[5] = {{1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}};
    RANGE_LIST_NODE nodes[5] = {{{1, 1}, 0, 0}, {{2, 1}, 0, 0}, {{3, 1}, 0, 0}, {{4, 1}, 0, 0}, {{5, 1}, 0, 0}};
    RANGE_LIST list;
    size_t i;
    enum { TEST_CASE_NUM = sizeof(ranges) / sizeof(RANGE)};
    //PRANGE_LIST_NODE p_next_node = NULL;
    //PRANGE_LIST_NODE p_prev_node = NULL;
    printf("%s run.\n", __FUNCTION__);
    list._node_size = sizeof(nodes) / sizeof(RANGE_LIST_NODE);
    assert (list._node_size > 2);
    list._head_node = &nodes[0];
    list._tail_node = &nodes[list._node_size - 1];
    list._head_node->_prev_node = NULL;
    list._head_node->_next_node = &nodes[1];
    list._tail_node->_prev_node = &nodes[list._node_size - 2];
    list._tail_node->_next_node = NULL;
    for (i = 1; i < list._node_size-1; ++i)
    {
        nodes[i]._prev_node = &nodes[i-1];
        nodes[i]._next_node = &nodes[i+1];
    }

    for (i = 0; i < TEST_CASE_NUM; i ++)
    {
        printf("test[%i]: result = %i\n", i+1,  range_list_is_head_relevant(&list, &ranges[i]));
	}
	printf("test[%i]: result = %i\n", i+1,  range_list_is_head_relevant(NULL, NULL));
    return 0;
}

int main(int argc, char* argv[])
{
    test_range_complete();
    test_range_list_is_head_relevant();

    return 0;
}

