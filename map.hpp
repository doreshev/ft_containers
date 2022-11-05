/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: doreshev <doreshev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/08 16:03:32 by doreshev          #+#    #+#             */
/*   Updated: 2022/11/05 18:40:14 by doreshev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MAP_HPP
# define MAP_HPP

# include <stdexcept>
# include "type_traits.hpp"
# include "iterators/tree.hpp"

namespace ft {
template<class Key, class T, class Compare = std::less<Key>, class Allocator = std::allocator<ft::pair<const Key, T> > >
class map {
public:
	typedef	Key											key_type;
	typedef	T											mapped_type;
	typedef	Compare										key_compare;
	typedef	typename ft::pair<const Key, T>				value_type;
	typedef	Allocator									allocator_type;
	typedef	typename allocator_type::reference			reference;
	typedef	typename allocator_type::const_reference	const_reference;
	typedef	typename allocator_type::pointer			pointer;
	typedef	typename allocator_type::const_pointer		const_pointer;
	typedef	typename allocator_type::size_type			size_type;
	typedef typename allocator_type::difference_type	difference_type;

	typedef	ft::tree<key_type, value_type, key_compare, allocator_type>	tree;
	typedef	typename tree::iterator					iterator;
	typedef	typename tree::const_iterator			const_iterator;
	typedef	typename tree::reverse_iterator			reverse_iterator;
	typedef	typename tree::const_reverse_iterator	const_reverse_iterator;

	class value_compare : std::binary_function<value_type, value_type, bool>
	{
	private:
		friend class map;
	protected:
		Compare		comp;
		value_compare	(Compare c) : comp(c) {}
	public:
		// typedef bool		result_type;
		// typedef value_type	first_argument_type;
		// typedef value_type	second_argument_type;

		bool	operator() (const value_type& x, const value_type& y) const {
			return comp(x.first, y.first);
	  }
	};

private:
	key_compare			_comp;
	allocator_type		_alloc;
	tree				_tree;
	
public:
// MEMBER FUNCTIONS
	// CONSTRUCTORS
		// 1) Empty
	explicit map (const key_compare& comp = key_compare(), const allocator_type& alloc = allocator_type())
				: _comp(comp), _alloc(alloc), _tree() { }
		// 2) Range
	template <class InputIterator> 
	map (InputIterator first, InputIterator last, const key_compare& comp = key_compare(),
		const allocator_type& alloc = allocator_type()) : _comp(comp), _alloc(alloc), _tree() {
		insert(first, last);
	}
		// 3) Copy
	map (const map& x) : _comp(x.key_comp()), _alloc(x.get_allocator()), _tree(x._tree) { }
	// DESTRUCTOR
	~map () { }
	// ASSIGN OPERATOR
		//assigns values to the container
	map& operator= (const map& x) {
		_alloc = x.get_allocator();
		_comp = x.key_comp();
		_tree = x._tree;
		return *this;
	}
	// ALLOCATOR GETTER -> Returns allocator
	allocator_type get_allocator() const { return _alloc; }

// ELEMENT ACCESS
	// AT -> access specified element with bounds checking
	mapped_type& at (const key_type& k) {
		Node<value_type>* tmp = _tree.find(k);
		if (tmp == nullptr)
			throw std::out_of_range("ft::map::at");
		return (tmp->value.second);
	}
	const mapped_type& at (const key_type& k) const {
		Node<value_type>* tmp = _tree.find(k);
		if (tmp == nullptr)
			throw std::out_of_range("ft::map::at");
		return (tmp->value.second);
	}
	// [] -> access or insert specified element
	mapped_type& operator[] (const key_type& key) {
		return (insert(ft::make_pair(key, T())).first)->second;
	}
// ITERATORS
	// 1) begin -> returns an iterator to the beginning
	iterator begin() {
		return iterator(_tree.begin());
	}
	const_iterator begin() const {
		return const_iterator(_tree.begin());
	}
	iterator end() {
		return iterator(_tree.end());
	}
	const_iterator end() const {
		return const_iterator(_tree.end());
	}
	reverse_iterator rbegin() {
		return reverse_iterator(_tree.end());
	}
	const_reverse_iterator rbegin() const {
		return const_reverse_iterator(_tree.end());
	}
	reverse_iterator rend() {
		return reverse_iterator(_tree.begin());
	}
	const_reverse_iterator rend() const {
		return const_reverse_iterator(_tree.begin());
	}
// CAPACITY
	// 1) empty -> checks whether the container is empty
	bool empty() const {
		if (_tree.head() == nullptr)
			return true;
		return false;
	}
	// 2) size -> Return container size
	size_type size() const { return _tree.size(); }
	// 3) maxsize -> Return container maximum size possible on current architecture
	size_type max_size() const { return _tree.max_size(); }
// MODIFIERS
	// 1) Removes all elements from the map
	void clear() { _tree.clear(); }
	// 2) Insertion of elements
		// a) Single element
	pair<iterator,bool> insert (const value_type& val) { return _tree.insert(val); }
		// b) With hint 
	iterator insert (iterator position, const value_type& val) {
		(void)position;
		return _tree.insert(val).first;
	}
		// c) Range
	template <class InputIterator>
	void insert (InputIterator first, InputIterator last) {
		for ( ; first != last; first++)
			insert(*first);
	}
	// 3) Erase -> Removes from container elements
		// a) Removes element in given position
	void erase (iterator position) { _tree.erase(position.base()); }
		// b) Removes element with given key
	size_type erase (const key_type& k) { return _tree.erase(k); }
		// c) Removes elemets in given range
	void erase (iterator first, iterator last) {
		for ( ; first != last; first++)
			erase(first);
	}
	// 4) Swap -> Exchanges the content of the container by the content of x
	void swap (map& x) {
		ft::swap(_comp, x.key_comp());
		ft::swap(_alloc, x.get_allocator());
		_tree->swap(x._tree);
	}
// LOOKUP (Operations)
	// 1) Count -> Count elements with a specific key
	size_type count (const key_type& k) const {
		return _tree.count(k);
	}
	// 2) Find -> finds element with specific key
	iterator find (const key_type& k) { return iterator(_tree.find(k)); }
	const_iterator find (const key_type& k) const { return const_iterator(_tree.find(k)); }
	// 3) Get range of equal elements
	pair<const_iterator,const_iterator> equal_range (const key_type& k) const {
		return ft::make_pair(lower_bound(k), upper_bound(k));
	}
	pair<iterator,iterator> equal_range (const key_type& k) {
		return ft::make_pair(lower_bound(k), upper_bound(k));
	}
	// 4) Return iterator to lower bound
	iterator lower_bound (const key_type& k) {
		iterator it = begin();
		for (iterator last = end(); it != last && !_comp(*it, k) && !_comp(k, *it); it++);
		return it;
	}
	const_iterator lower_bound (const key_type& k) const {
		const_iterator it = begin();
		for (const_iterator last = end(); it != last && !_comp(*it, k) && !_comp(k, *it); it++);
		return it;
	}
	// 5) Return iterator to upper bound
	iterator upper_bound (const key_type& k) {
		iterator it = end();
		for (iterator first = begin(); it != first && !_comp(*it, k) && !_comp(k, *it); it--);
		return it;
	}
	const_iterator upper_bound (const key_type& k) const {
		const_iterator it = end();
		for (const_iterator first = begin(); it != first && !_comp(*it, k) && !_comp(k, *it); it--);
		return it;
	}
// OBSERVERS
	// Returns the function that compares keys
	key_compare key_comp() const { return _comp; }
};

// RELATIONAL OPERATORS MAP
template< class Key, class T, class Compare, class Alloc >
bool operator==( const ft::map<Key,T,Compare,Alloc>& lhs, const ft::map<Key,T,Compare,Alloc>& rhs ) {
	if (lhs.size() != rhs.size())
		return false;
	return ft::equal(lhs.begin(), lhs.end(), rhs.begin());
}
template <class Key, class T, class Compare, class Alloc> 
bool operator!= ( const map<Key,T,Compare,Alloc>& lhs, const map<Key,T,Compare,Alloc>& rhs ) {
	return !(lhs == rhs);
}
template <class Key, class T, class Compare, class Alloc>
bool operator< ( const map<Key,T,Compare,Alloc>& lhs, const map<Key,T,Compare,Alloc>& rhs ) {
	return ft::lexicographical_compare (lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}
template <class Key, class T, class Compare, class Alloc> 
bool operator<= ( const map<Key,T,Compare,Alloc>& lhs, const map<Key,T,Compare,Alloc>& rhs ) {
	return 	!(rhs < lhs);
}
template <class Key, class T, class Compare, class Alloc> 
bool operator>  ( const map<Key,T,Compare,Alloc>& lhs, const map<Key,T,Compare,Alloc>& rhs ) {
	return rhs < lhs;
}
template <class Key, class T, class Compare, class Alloc>
bool operator>= ( const map<Key,T,Compare,Alloc>& lhs, const map<Key,T,Compare,Alloc>& rhs ) {
	return 	!(lhs < rhs);
}
//SWAP
template <class Key, class T, class Compare, class Alloc>
void swap (map<Key,T,Compare,Alloc>& lhs, map<Key,T,Compare,Alloc>& rhs) { lhs.swap(rhs); }

}

#endif