/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tree.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: doreshev <doreshev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/08 16:06:43 by doreshev          #+#    #+#             */
/*   Updated: 2022/11/05 18:42:21 by doreshev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TREE_HPP
# define TREE_HPP

# include "../utility.hpp"
# include "TreeIterator.hpp"
# include "../algorithm.hpp"

namespace ft {
//TREE IMPLEMENTATION
template<class Key, class T, class Compare, class Allocator>
class tree {
public:
	typedef T																	value_type;
	typedef Key																	key_type;
	typedef Node<T>																node_type;
	typedef node_type*															pointer;
	typedef Compare																value_compare;
	typedef Allocator															allocator_type;
	typedef typename Allocator::template rebind<Node<T> >::other				node_allocator;
	typedef typename allocator_type::size_type									size_type;
	typedef typename ft::TreeIterator<value_type, node_type> 					iterator;
	typedef typename ft::TreeIterator<const value_type, const Node<T> >			const_iterator;
	typedef typename ft::reverse_iterator<iterator>								reverse_iterator;
	typedef typename ft::reverse_iterator<const_iterator>						const_reverse_iterator;

protected:
	allocator_type			_alloc;
	node_allocator			_node_alloc;
	value_compare			_compare;
	pointer					_root;
	pointer					_head;

public:
	//  CONSTRUCTORS AND DESTRUCTOR
	tree(const value_compare& compare = value_compare(), const allocator_type& alloc = allocator_type())
		: _alloc(alloc), _node_alloc(alloc), _compare(compare), _root(_null_leaf_alloc()), _head(nullptr) { }
	tree (const tree& x) :	_alloc(x._alloc), _node_alloc(x._node_alloc), _compare(x._compare),
							_root(x._root), _head(x._head) { }
	~tree() { 
		remove_tree();
		_node_alloc.deallocate(_head, 1);
	}
	tree& operator= (const tree& x) {
		_alloc = x._alloc;
		_node_alloc = x._node_alloc;
		_compare = x._compare;
		_root = x._root;
		_head = x._head;
		return *this;
	}

	size_type	max_size () const { return _node_alloc.max_size(); }
	size_type	size () const {
		size_type result = 0;
		for (pointer tmp = node_minimum(_head); tmp != _root; tmp = successor(tmp))
			result++;
		return result;
	}
	// 1)Insertion of single element
	ft::pair<iterator, bool> insert(const value_type& val) {
		if (_head == nullptr) {
			_head = _nallocate(val);
			_head->red = false;
			_head->parent = _root;
			_root->left = _head;
			return ft::make_pair(iterator(_head), true);
		}
		for ( pointer tmp = _head; tmp != nullptr; ) {
			if (!_compare(tmp->value.first, val.first)) {
				if (!_compare(val.first, tmp->value.first))
					return ft::make_pair(iterator(tmp), false);
				if (tmp->left == nullptr) {
					tmp->left = _nallocate(val);
					tmp->left->parent = tmp;
					_ins_balance(tmp->left, true);
					return ft::make_pair(iterator(tmp->left), true);
				}
				tmp = tmp->left;
			}
			else {
				if (tmp->right == nullptr) {
					tmp->right = _nallocate(val);
					tmp->right->parent = tmp;
					_ins_balance(tmp->right, false);
					return ft::make_pair(iterator(tmp->right), true);
				}
				tmp = tmp->right;
			}
		}
		return (ft::make_pair(iterator(_root), false));
	}
	pointer end () const { return _root; }
	pointer begin () const {
		if (_head == nullptr)
			return _root;
		return node_minimum(_head); 
	}
	pointer head () const {return _head; }
	// 2) Find
	pointer find(const key_type& key) const {
		for ( pointer tmp = _head; tmp != nullptr; ) {
			if (!_compare(tmp->value.first, key)) {
				if (!_compare(key, tmp->value.first))
					return tmp;
				if (tmp->left == nullptr)
					return nullptr;
				tmp = tmp->left;
			}
			else {
				if (tmp->right == nullptr) {
					return nullptr;
				}
				tmp = tmp->right;
			}
		}
		return nullptr;
	}
	// 3) Deletion
	void	erase (pointer pos) {
		if (pos == nullptr || pos == _root)
			return ;
		_rb_deletion(pos);
	}
	size_type erase (const key_type& k) {
		if (pos == nullptr || pos == _root || find(k) == _root)
			return 0;
		_rb_deletion(pos);
		return 1;
	}
	void	clear() {
		for (pointer tmp = node_minimum(_head); tmp != _root; tmp = successor(tmp)) {
			try	{
				_alloc.destroy(&(tmp->value));
			}
			catch(...) {
				remove_tree();
			}
		}
		_head = nullptr;
	}
	// 4) Swap
	void swap (tree& x) {
		ft::swap(_root, x._root);
		ft::swap(_head, x._head);
		ft::swap(_node_alloc, x._node_alloc);
		ft::swap(_alloc, x._alloc);
		ft::swap(_compare, x._compare);
	}
	// 5) Count
	size_type count (const key_type& k) const {
		if ( find(k) == _root)
			return 0;
		return 1;
	}
	// 6) lower/upper bound
		
	// Min/Max search functions
	pointer	node_maximum (pointer current) const {
		while (current->right != nullptr)
			current = current->right;
		return current;
	}
	pointer	node_minimum (pointer current) const {
		while (current && current->left != nullptr)
			current = current->left;
		return current;	
	}
	// Predecessor/successor functions
	pointer	successor (pointer current) const {
		if (current->right != nullptr) {
			return node_minimum(current->right);
		}
		pointer	Par = current->parent;
		while (Par != _root && current == Par->right) {
			current = Par;
			Par = Par->parent;
		}
		return Par;
	}
	pointer	predecessor (pointer current) const {
		if (current->left != nullptr) {
			return node_maximum(current->left);
		}
		pointer	Par = current->parent;
		while (Par != _root && current == Par->left) {
			current = Par;
			Par = Par->parent;
		}
		return Par;
	}
private:
	//TREE MANIPULATION
		//1)Left Rotation
	void	rotate_left(pointer x) {
		pointer y = x->right;

		if (y) {
			x->right = y->left;
			if (y->left)
				y->left->parent = x;
			y->left = x;
			y->parent = x->parent;
			if (x->parent) {
				if (x->parent->left == x)
					x->parent->left = y;
				else
					x->parent->right = y;
			}
			else
				_head = y;
			x->parent = y;
		}
	}
		//2)Right Rotation
	void	rotate_right(pointer x) {
		pointer y = x->left;

		if (y) {
			x->left = y->right;
			if (y->right)
				y->right->parent = x;
			y->right = x;
			y->parent = x->parent;
			if (x->parent) {
				if (x->parent->left == x)
					x->parent->left = y;
				else
					x->parent->right = y;
			}
			else
				_head = y;
			x->parent = y;
		}
	}
		//3)Recolour
	void	recolor_node(const node_type& x) {
		if (x.red == true)
			x.red = false;
		else
			x.red = true;
	}
	void remove_tree() {
		if (_head == nullptr)
			return;
		for (pointer tmp = node_minimum(_head); tmp != _root; tmp = successor(tmp)) {
			try	{
				_del_node(tmp);
			}
			catch(...) {
				if (_head != nullptr)
					remove_tree();
			}
		}
		_head = nullptr;
	}
	// UTILS
		// 1) Node Allocation
			// a) red node allocation
	pointer	_nallocate (const value_type& val) {
		pointer new_node = _node_alloc.allocate(1);
		try	{
			_alloc.construct(&(new_node->value), val);
		}
		catch(...) {
			_node_alloc.deallocate(new_node, 1);
		}
		new_node->red = true; new_node->left = nullptr;
		new_node->right = nullptr; new_node->parent = nullptr;
		return (new_node);
	}
			// b) Null leaf allocation
	pointer	_null_leaf_alloc() {
		pointer new_node = _node_alloc.allocate(1);
		new_node->red = false; new_node->left = nullptr;
		new_node->right = nullptr; new_node->parent = nullptr;
		return (new_node);
	}
		// 2) Delete Node
	void	_del_node (pointer pos) {
		try	{
			_alloc.destroy(&(pos->value));
		}
		catch(...) {
			_node_alloc.deallocate(pos, 1);
		}
		_node_alloc.deallocate(pos, 1);
	}
		// 3) Balance Tree after Insertion
	void	_ins_balance (pointer Kid, bool left_child) {
		for (pointer Uncle = nullptr; Kid != _head && Kid->parent->red == true; ) {
			if (Kid->parent == Kid->parent->parent->right) {
				Uncle = Kid->parent->parent->left;
				if (Uncle && Uncle->red == true) {
					_ins_balance_case1(Kid, Uncle);
					Kid = Kid->parent->parent;
				}
				else {
					if (left_child == true) {
						Kid = Kid->parent;
						rotate_right(Kid);
					}
					_ins_balance_case2(Kid);
				}
			}
			else {
				Uncle = Kid->parent->parent->right;
				if (Uncle && Uncle->red == true) {
					_ins_balance_case1(Kid, Uncle);
					Kid = Kid->parent->parent;
				}
				else {
					if (left_child == false) {
						Kid = Kid->parent;
						rotate_left(Kid);
					}
					_ins_balance_case3(Kid); ///////////////////////////$test
				}
			}
		}
	}
	// 4) Balncing insertion cases
		 // If Uncle is red colour -> colors to be flipped -> Uncle and Parent become black, Grandparent red
	void	_ins_balance_case1(pointer Kid, pointer Uncle) {
		Kid->parent->red = false;
		Uncle->red = false;
		if (Kid->parent->parent != _root)
			Kid->parent->parent->red = true;
	}
		 //b) Uncle is black, Parent is right child and kid is right child
	void	_ins_balance_case2(pointer Kid) {
		Kid->parent->red = false;
		Kid->parent->parent->red = true;
		rotate_left(Kid->parent->parent);
	}
		 //c) Uncle is black, Parent is right child and kid is left child
	void	_ins_balance_case3(pointer Kid) {
		Kid->parent->red = false;
		Kid->parent->parent->red = true;
		rotate_right(Kid->parent->parent);
	}
	// 5) Deleting Node from tree
	void	_rb_deletion(pointer z) {
		bool	is_red = z->red;
		pointer x;
		pointer y = z;

		if (z->left == nullptr) {
			x = z->right;
			_del_changenodes(z, z->right);
		}
		else if (z->right == nullptr) {
			x = z->left;
			_del_changenodes(z, z->left);
		}
		else {
			y = node_minimum(z->right);
			is_red = y->red;
			x = y->right;
			if (y->parent == z)
				x->parent = y;
			else {
				_del_changenodes(y, y->right);
				y->right = z->right;
				y->right->parent = y;
			}
			_del_changenodes(z, y);
			y->left = z->left;
			y->left->parent = y;
			y->red = z->red;
		}
		_del_node(z);
		if (is_red == false)
			_del_rebalance(x);
	}
		 // a) Rebalancing tree after deletion
	void _del_rebalance(pointer x) {
		for (pointer s = nullptr; x != _root && x->red == false;) {
			if (x == x->parent->left) {
				s = x->parent->right;
				if (s->red == true) {
					s->red = false;
					x->parent->red = true;
					rotate_left(x->parent);
					s = x->parent->right;
				}
				if (s->left->red == false && s->right->red == false) {
					s->red = true;
					x = x->parent;
				}
				else {
					if (s->right->red == false) {
						s->left->red = false;
						s->red = true;
						rotate_right(s);
						s = x->parent->right;
					}
					s->red = x->parent->red;
					s->parent->red = false;
					s->right->red = false;
					rotate_left(x->parent);
					x = _root;
				}
			}
			else {
				s = x->parent->left;
				if (s->red == true) {
					s->red = false;
					x->parent->red = true;
					rotate_right(x->parent);
					s = x->parent->left;
				}
				if (s->right->red == false && s->left->red == false) {
					s->red = true;
					x = x->parent;
				}
				else {
					if (s->left->red == false) {
						s->right->red = false;
						s->red = true;
						rotate_left(s);
						s = x->parent->left;
					}
					s->red = x->parent->red;
					x->parent->red = false;
					s->left->red = false;
					rotate_right(x->parent);
					x = _root;
				}
			}
		}
		x->red = false;
	}
	// Replaces node 'pos' with a node 'other_pos'
	void _del_changenodes(pointer pos, pointer other_pos) {
		if (pos->parent == _root)
			_head = pos;
		else if (pos == pos->parent->left)
			pos->parent->left = other_pos;
		else
			pos->right->parent = other_pos;
		other_pos->parent = pos->parent;
	}
};
}

#endif