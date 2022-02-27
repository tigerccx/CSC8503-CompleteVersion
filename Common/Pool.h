#pragma once
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <functional>

using namespace std;


template<class T> class Pool;

//template<class T>
//class PoolEntry {
//public:	
//	PoolEntry() : object(new T()){ }
//	~PoolEntry() { delete object; }
//	T* object;
//
//	//unsigned int GetID() const {
//	//	return id;
//	//}
//
//	//friend class Pool;
//
////protected:
////	unsigned int id;
//};

class InvalidPoolResource : public std::logic_error
{
public:
	InvalidPoolResource() : std::logic_error("Not a valid pool resource! Do not create pool resources elsewhere! ") { };
};

template<class T>
class Pool
{
public:
	Pool() { ExpandPool(); }
	~Pool() {
		//cout << "DELETE Pool" << endl;
		for (auto& entry : listActive) {
			delete entry;
		}
		for (auto& entry : listInctive) {
			delete entry;
		}
	}

	typedef std::function<void(T*)> PoolResourceProcFunc;

	int expansionCount = 10;
	PoolResourceProcFunc funcInitResource = nullptr;
	//T defaultVal;

	/*PoolEntry<T>* GetResource() {
		if (listInctive.size() == 0) {
			ExpandPool();
		}
		PoolEntry<T>* r = listInctive.back();
		listInctive.pop_back();
		listActive.emplace_back(r);
		return r;
	}

	bool PutResource(PoolEntry<T>* r) {
		auto itr = find(listActive.begin(), listActive.end(), r);
		if (itr == listActive.end()) {
			return false;
		}
		listActive.erase(itr);
		listInctive.emplace_back(r);
		return true;
	}

	void ExpandPool() {
		for (int i = 0; i < expansionCount; ++i) {
			listInctive.emplace_back(new PoolEntry<T>());
		}
	}*/

	T* GetResource() {
		if (listInctive.size() == 0) {
			ExpandPool();
		}
		T* r = listInctive.back();
		listInctive.pop_back();
		listActive.emplace_back(r);
		return r;
	}

	bool PutResource(T* r) {
		auto itr = find(listActive.begin(), listActive.end(), r);
		if (itr == listActive.end()) {
			throw InvalidPoolResource();
		}
		listActive.erase(itr);
		if (funcInitResource)
			funcInitResource(r);
		listInctive.emplace_back(r);
		return true;
	}

	void ExpandPool() {
		for (int i = 0; i < expansionCount; ++i) {
			listInctive.emplace_back(new T());
		}
	}

protected:
	vector<T*> listActive;
	vector<T*> listInctive;
};

