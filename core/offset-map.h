#ifndef SOREN_OFFSET_MAP_INCLUDED
#define SOREN_OFFSET_MAP_INCLUDED

#include "core/types.h"

#include <vector>
#include <string>

#include <algorithm>

namespace soren {

template<typename ValueType>
struct OffsetMap : public std::vector<std::pair<unsigned, ValueType>>
{
	// this is just an abstraction behind a sorted vector
	// maybe I should just have used std::map

	using _super = std::vector<std::pair<unsigned, ValueType>>;

	using iterator = typename _super::iterator;
	using const_iterator = typename _super::const_iterator;

	using _super::_super; // TODO

	void set(unsigned offset, ValueType&& value)
	{
		auto pair = std::make_pair(offset, std::move(value));
		auto upit = std::upper_bound(_super::begin(), _super::end(),
			pair, [] (auto& a, auto& b) { return a.first < b.first; });

		_super::insert(upit, std::move(pair));
	}

	void set(unsigned offset, const ValueType& value)
	{
		auto pair = std::make_pair(offset, value);
		auto upit = std::upper_bound(_super::begin(), _super::end(),
			pair, [] (auto& a, auto& b) { return a.first < b.first; });

		_super::insert(upit, std::move(pair));
	}

	iterator get(unsigned offset)
	{
		auto index = get_index(offset);

		if (index == bad_index)
			return _super::end();

		return _super::begin() + index;
	}

	const_iterator get(unsigned offset) const
	{
		auto index = get_index(offset);

		if (index == bad_index)
			return _super::cend();

		return _super::cbegin() + index;
	}

	bool has(unsigned offset) const
	{
		return get_index(offset) != bad_index;
	}

	template<typename Func>
	void for_at(unsigned offset, Func func) const
	{
		auto index = get_index(offset);

		if (index != bad_index)
			func(_super::operator[](index).second);
	}

	std::size_t get_index(unsigned offset) const
	{
		if (!_super::empty())
		{
			// binary search yay

			std::ptrdiff_t left = 0;
			std::ptrdiff_t right = _super::size() - 1;

			while (left <= right)
			{
				std::size_t mid = (left + right) / 2;
				auto& pair = _super::operator[](mid);

				if (pair.first < offset)
				{
					left = mid+1;
					continue;
				}

				if (pair.first > offset)
				{
					right = mid-1;
					continue;
				}

				return mid;
			}
		}

		return bad_index;
	}

	static constexpr std::size_t bad_index = std::numeric_limits<std::size_t>::max();

protected:
	// this doesn't prevent *all* other modifying operations, but it will prevent common ones
	using _super::insert;
	using _super::emplace;
	using _super::push_back;
	using _super::emplace_back;
};

using NameMap = OffsetMap<std::string>;

template<typename Idx>
struct IndexSet
{
	using index_t = Idx;

	void insert(Idx val)
	{
		if (val >= values.size())
			values.resize(val+1, false);

		values[val] = true;
	}

	void remove(Idx val)
	{
		if (val < values.size())
			values[val] = false;
	}

	bool contains(Idx val) const { return val < values.size() && values[val]; }

	struct Iterator
	{
		Iterator(const IndexSet& parent, Idx val)
			: parent(parent), value(val) {}

		bool operator == (const Iterator& other) const { return value == other.value; }
		bool operator != (const Iterator& other) const { return value != other.value; }

		Idx operator * () const { return value; }

		Iterator operator ++ () { while (++value < parent.values.size() && !parent.values[value]) {} return *this; }
		Iterator operator ++ (int) { Iterator it = *this; ++*this; return it; }

	private:
		const IndexSet& parent;
		Idx value;
	};

	using iterator = Iterator;
	using const_iterator = Iterator;

	Iterator begin() const { return Iterator(*this, first_index()); }
	Iterator end() const { return Iterator(*this, values.size()); }

private:
	Idx first_index() const
	{
		Idx result = 0;

		while (result < values.size() && !values[result])
			result++;

		return result;
	}

private:
	std::vector<bool> values;
};

} // namespace mary

#endif // SOREN_OFFSET_MAP_INCLUDED
