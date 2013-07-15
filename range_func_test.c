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
	函数功能：
		比较两个range的大小，具体的比较方法参见返回值消息描述。
	返回值：
		lval与rval相等，返回0；
		两者交叉且lval在rval右边，返回1；
		两者交叉且lval在rval左边，返回-1；
		lval被rval包含，返回2；
		rval被lval包含，返回-2；
		两者无交集且lval在右边，返回3；
		两者无交集且rval在右边，返回-3；
		参数错误返回-4
***/
_i32 range_complete(const RANGE *lval, const RANGE *rval)
{
    _u32 lval_begin;
    _u32 rval_begin;
    _u32 lval_end;
    _u32 rval_end;

	//检验有效性
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
	函数功能：
		计算range_list与range，判断是否首尾重叠。
	返回值：
		 -1 表示参数错误
		 0  表示r 与range_list 中一项在头部重叠
		 1  表示r 与range_list 中一项在尾部重叠
		 2  表示r 与range_list 无重叠
		 注：若r与range_list首尾都重叠，则优先判断头部重叠。
*/
_i32 range_list_is_head_relevant(RANGE_LIST* range_list, const RANGE* r)
{
    RANGE_LIST_NODE *cur_node = NULL;
    int complete_res;
    // 检验有效性
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

