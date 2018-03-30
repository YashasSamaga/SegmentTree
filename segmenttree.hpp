#ifndef INC_SEGMENTTREE_HPP
#define INC_SEGMENTTREE_HPP

#include <vector> //std::vector
#include <functional> //std::plus
#include <algorithm> //std::move_backward, std::copy_backward
#include <utility> //std::move
#include <iterator>

namespace {
	/*
	**
	** T - type of elements being stored
	** 	
	** Container: The type of the underlying container to use to store the elements. 
	** 			  The container must satisfy the requirements of SequenceContainer, and its iterators must satisfy the requirements of RandomAccessIterator.
	**
	** BinaryOperation: operation of the segment tree
	*/
	template <typename T, class Container = std::vector<T>, class BinaryOperation = std::plus<T>>
	class SegmentTree {
		public:
			typedef T		 			value_type;
			typedef Container 			container_type;
			typedef BinaryOperation 	operation;
			
			static_assert(std::is_same<T, typename Container::value_type>::value, "SegmentTree: 'T' must be of the same type as Container::value_type");
			
			/* `reference` is publicly accessible member class */
			typedef	typename Container::const_reference 		const_reference;
			/* `iterator` is publicly accessible member class */
			typedef typename Container::const_iterator 			const_iterator;
			//typedef typename std::reverse_iterator<iterator>	reverse_iterator;
			typedef typename Container::const_reverse_iterator	const_reverse_iterator;
			typedef	typename Container::difference_type 		difference_type;
			typedef	typename Container::size_type 				size_type;

			/*
			** Constructors
			*/
			SegmentTree(const BinaryOperation& op, const Container& cont) : op(op) {
				c.resize(2*cont.size());
				std::copy_backward(cont.begin(), cont.end(), c.end());
				build();
			}
			
			explicit SegmentTree(const BinaryOperation& op = BinaryOperation(), Container&& cont = Container()) : op(op) {
				c.resize(2*cont.size());
				std::move_backward(cont.begin(), cont.end(), c.end());
				build();
			}
			
			explicit SegmentTree(const Container& other) {
				c.resize(2*other.size());
				std::copy_backward(other.begin(), other.end(), c.end());
				build();
			}

			SegmentTree(const SegmentTree& other) : c(other.c), op(other.op) { }
			SegmentTree(SegmentTree&& other) : c(std::move(other.c)), op(std::move(other.op)) { }				
			
			template <class Allocator>
			SegmentTree(const Container& other, const Allocator& alloc) : c(alloc) {
				c.resize(2*other.size());
				std::copy_backward(other.begin(), other.end(), c.end());
				build();
			}
			
			template <class Allocator>
			SegmentTree(Container&& other, const Allocator& alloc) : c(alloc) {
				c.resize(2*other.size());
				std::move_backward(other.begin(), other.end(), c.end());
				build();
			}
			
			template <class Allocator>
			SegmentTree(const BinaryOperation& op, const Container& other, const Allocator& alloc) : op(op), c(alloc) {
				c.resize(2*other.size());
				std::copy_backward(other.begin(), other.end(), c.end());
				build();
			}
			
			template <class Allocator>
			SegmentTree(const BinaryOperation& op, Container&& other, const Allocator& alloc) : op(op), c(alloc) {
				c.resize(2*other.size());
				std::move_backward(other.begin(), other.end(), c.end());
				build();
			}
		
			template <class InputItr>
			SegmentTree(InputItr first, InputItr last, const BinaryOperation& op = BinaryOperation()) : op(op) {
				c.resize(2*std::distance(first, last));
				std::copy_backward(first, last, c.end());
				build();
			}

			~SegmentTree() { }

			/*
			** Assignment Operator
			*/
			SegmentTree& operator=(const SegmentTree& other) {
				op = other.op;
				c = other.c;
			}

			SegmentTree& operator=(SegmentTree&& other) {
				op = std::move(other.op);
				c = std::move(other.c);
				return *this;	
			}

			/*
			** Iterator functions
			*/
			auto begin() 	const { return c.begin() 	+ c.size()/2;	}
			auto end() 		const { return c.end();						}
			
			auto cbegin()	const { return c.cbegin() 	+ c.size()/2; 	}
			auto cend() 	const {	return c.cend();				 	}
			
			auto rbegin() 	const { return c.rbegin();				 	}
			auto rend() 	const { return c.rend() 	+ c.size()/2; 	}
			
			auto crbegin() 	const { return c.crbegin();					}
			auto crend() 	const { return c.crend() 	+ c.size()/2; 	}
			
			/*
			** Member functions
			*/
			[[nodiscard]] size_type size() 	const { return c.size()/2; }
			[[nodiscard]] bool empty() 		const { return c.empty(); }

		protected:
			void build() {
				for(int i = c.size()/2 - 1; i > 0; i--)
					c[i] = op(c[i*2], c[i*2 + 1]);
			}
			
		public:
			void update(size_type idx, T value) {
				idx += c.size()/2;
				c[idx] = std::move(value); //TODO move?
				while(idx > 1) {
					if(idx % 2 == 0)
						c[idx/2] = op(c[idx], c[idx + 1]);
					else
						c[idx/2] = op(c[idx - 1], c[idx]);
					idx /= 2;
				}	
			}

			T query(size_type left, size_type right) const {
				T result_left, result_right;
				for (left += c.size()/2, right += c.size()/2; left < right; left /= 2, right /= 2) {
					if (left % 2) 
						result_left = op(result_left, c[left++]);			 
					if (right % 2) 
						result_right = op(c[--right], result_right);
				}	 
				return op(result_left, result_right);
			}		

			/*
			** [] operator
			*/
			class reference;
			reference operator[](size_type idx) { return reference(this, idx); }			
			const_reference operator[](size_type idx) const { return c[c.size()/2 + idx]; }
			
			/*
			** at operator ([] operator with bounds checking)
			*/
			reference at(size_type idx) {
				if(idx >= c.size()/2)
					throw std::out_of_range("SegmentTree: subscript out of range"); //TODO: fix the throw
				return reference(this, idx);
			}
			const_reference at(size_type idx) const {
				if(idx >= c.size()/2)
					throw std::out_of_range("SegmentTree: subscript out of range"); //TODO: fix the throw
				return c[c.size()/2 + idx];
			}
	
			/*
			** swap
			*/	
			void swap(SegmentTree& other) noexcept {
				std::swap(c, other.c);
				std::swap(op, other.op);		
			}	
			
			/*
			** proxy pattern for monitoring updates to references
			*/
			class reference {
				private:
					friend SegmentTree;
					reference(SegmentTree *st, int idx) : _parent(st), _index(idx) { }
					
				public:
					reference& operator=(const T& value) {
						_parent->update(_index, value);
						return *this;
					}

					reference& operator=(const reference& other) const {
						_index = other._index;
						_parent = other._parent;
					}

					operator T() const noexcept {
						return _parent->c[_index];
					}
					
				protected:
					size_type _index;
					SegmentTree<T, Container, BinaryOperation> *_parent;
			}; /* reference */			
			
			class iterator {
		public:
			typedef std::random_access_iterator_tag	iterator_category;
			typedef typename Container::value_type value_type;
   			typedef typename Container::difference_type difference_type;
   			//typedef              pointer;
   			//typedef  const T&    reference;
   					
   			iterator(SegmentTree *st, int idx) : _parent(st), _index(idx) { }
   			iterator(const iterator& other) { }
   			iterator operator=(const iterator& other) { }
   			~iterator() { }
  				   
   		protected:
   			size_type _index;
   			SegmentTree<T, Container, BinaryOperation> *_parent;
	}; /* iterator */
		protected:		
				Container c;
				BinaryOperation op;
	}; /* SegmentTree */

	template <typename T, class Container = std::vector<T>, class BinaryOperation = std::plus<T>>
	void swap(SegmentTree<T, Container, BinaryOperation>& lhs, SegmentTree<T, Container, BinaryOperation>& rhs) noexcept {
		noexcept(noexcept(lhs.swap(rhs)));
	} //TODO
} /* namespace */
#endif /* INC_SEGMENTTREE_HPP */
