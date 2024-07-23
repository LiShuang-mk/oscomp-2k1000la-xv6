//
// Created by Li Shuang ( pseudonym ) on 2024-07-23 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

#include "klib/function.hh"

namespace klib
{
	/// @brief 通用的二分查找算法
	/// @details 算法保证comp的调用满足：
	///              返回值为0表示匹配成功，小于0则target位于
	///              middle前面，大于0则target位于middle后面
	/// @tparam T 数组类型
	/// @tparam S 查找目标类型
	/// @param first 数组起始元素（指针）
	/// @param last 数组末尾元素（指针）
	/// @param target 查找目标（指针）
	/// @param comp 比较函数，返回值 -x,0,+x 三种情况分别对应匹配，目标位于左侧，目标位于右侧
	/// @return 匹配target的数组元素
	template<typename T, typename S>
	T * binary_search( T * first, T * last, S * target, std::function<int( T * middle, S * target ) comp )
	{
		long len;
		T *mid;
		int comp_res;
		while ( last >= first )
		{
			len = last - first + 1;
			mid = first + ( len / 2 );
			comp_res = comp( mid, target );
			if ( comp_res < 0 )
				last = mid - 1;
			else if ( comp_res > 0 )
				first = mid + 1;
			else
				return mid;
		}
		return nullptr;
	}

} // namespace klib
