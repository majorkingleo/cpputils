/**
 * FastDelivery
 * @author Copyright (c) 2023 Martin Oberzalek
 */
#pragma once
#ifndef FASTDELIVERY_H_
#define FASTDELIVERY_H_

#include <atomic>
#include <list>
#include <array>
#include <stdexcept>

namespace Tools {
namespace FastDelivery {

struct NodeListEntry
{
	// will be true if construction, destruction, or delivery is in progress
	// on any operation wait until it's false again
	std::atomic<bool> operation_in_progress = false;
	std::atomic<void*> node = nullptr;

	NodeListEntry( void *node_ )
	: node( node_ )
	{

	}
};


template <class DataType>
class PublisherNode
{
public:
	typedef DataType value_type;

protected:
	NodeListEntry *registry = nullptr;

public:
	~PublisherNode() {
		if( registry ) {
			bool expected = false;
			while(!registry->operation_in_progress.compare_exchange_weak( expected, true )) {
				expected = false; // reset: we only want to swap false→true
			}

			registry->node.store( nullptr );

			registry->operation_in_progress.store( false );
			registry = nullptr;
		}
	}

	void setRegistry( NodeListEntry *registry_ ) {
		registry = registry_;
	}
};

template <class DataType, class Node=PublisherNode<DataType>, class List=std::list<NodeListEntry>>
class Publisher
{
protected:
	List nodes;

public:
	Publisher() = default;

	// you are not allowed to copy it, because the nodes have
	// references to the nodes list
	Publisher( const Publisher & other ) = delete;
	Publisher & operator=( const Publisher & other ) = delete;

	void subscribe( Node *node ) {
		NodeListEntry & nle = nodes.emplace_back( node );
		node->setRegistry( &nle );
	}

	void unsubscribe( Node *node ) {
		for( auto it = nodes.begin(); it != nodes.end(); ++it ) {
			NodeListEntry & nle = *it;

			if( nle.node == node ) {
				bool expected = false;
				while(!nle.operation_in_progress.compare_exchange_weak( expected, true )) {
					expected = false; // reset: we only want to swap false→true
				}

				nle.node.store( nullptr );

				// Clear the node's back-pointer so ~PublisherNode()
				// won't try to access this (soon-to-be-freed) entry.
				node->setRegistry( nullptr );

				nodes.erase( it );
				return;
			}
		}
	}

	void distribute( const DataType & data ) {
		for( auto & nle : nodes ) {
			bool expected = false;
			if( !nle.operation_in_progress.compare_exchange_weak( expected, true ) ) {
				continue; // someone else holds it — skip this node
			}

			void *node = nle.node;

			if( node ) {
				Node *n = (Node*)node;
				n->deliver( data );
			}

			nle.operation_in_progress.store( false );
		}
	}
};

template <size_t N>
class StaticNodeList {
public:
	typedef typename std::array<NodeListEntry, N>::value_type value_type;
	typedef typename std::array<NodeListEntry, N>::reference reference;
	typedef typename std::array<NodeListEntry, N>::const_reference const_reference;
	typedef typename std::array<NodeListEntry, N>::size_type size_type;

	static constexpr size_t npos = N;

	class iterator
	{
	private:
		StaticNodeList<N> * data = nullptr;
		size_t pos = 0;

	public:
		iterator( StaticNodeList<N> * data_, size_t pos_ )
		: data( data_ ),
		  pos( pos_ )
		{}

		bool operator!=( const iterator & other_it ) const {
			if( pos == other_it.pos ) {
				return false;
			}

			return true;
		}

		NodeListEntry & operator*() {
			return data->at(pos);
		}

		const NodeListEntry & operator*() const {
			return data->at(pos);
		}

		iterator & operator++() {
			pos++;
			return *this;
		}

		iterator & operator--() {
			pos--;
			return *this;
		}
	};

public:

	~StaticNodeList() {
		while (size()) {
			pop_front();
		}
	}

	void push_back(const NodeListEntry& v) {
		if (size_ + 1 > N) {
			throw;
		}
		new (&array_[(front_ + size_) % N]) NodeListEntry(v);
		++size_;
	}

	void push_back(const NodeListEntry& v, bool discard_front ) {
		if (size_ + 1 > N) {
			pop_front();
		}
		new (&array_[(front_ + size_) % N]) NodeListEntry(v);
		++size_;
	}

	void pop_front() {
		if (size_ < 1) {
			throw;
		}

		front().~NodeListEntry();
		++front_;
		--size_;
		if (front_ >= N)
			front_ = 0;
	}

	const_reference front() const {
		return array_[front_];
	}

	reference front() {
		return array_[front_];
	}

	size_type size() const {
		return size_;
	}

	bool empty() const {
		return (size() == 0);
	}

	const_reference operator[]( size_t n ) const {
		return array_[(front_+n)%N];
	}

	reference operator[]( size_t n ) {
		return array_[(front_+n)%N];
	}

	reference at( size_t pos ) {
		if (pos >= size()) {
			throw std::out_of_range("out of range");
		}
		return array_[(front_+pos)%N];
	}

	const_reference at( size_t pos ) const {
		if (pos >= size()) {
			throw std::out_of_range("out of range");
		}
		return array_[(front_+pos)%N];
	}

	iterator begin() {
		return iterator(this,0);
	}

	iterator end() {
		return iterator(this,size_);
	}

private:
	size_type front_ = 0;
	size_type size_ = 0;
	std::array<NodeListEntry, N> array_;
};

} // namespace FastDelivery
} // namespace Tools

#endif /* FASTDELIVERY_H_ */
