#ifndef SOCCER_CONTAINER_H
#define SOCCER_CONTAINER_H

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace Soccer {

template<class T>
class Container {
	public:
		Container<T>(const char* containerName);
		virtual ~Container<T>() { }
		void addT(std::shared_ptr<T> t);
		std::shared_ptr<T> getT(const std::string& n);
		const std::string& getName() const;
		std::map<std::string, std::shared_ptr<T>>& getContainer();
		const std::map<std::string, std::shared_ptr<T>>& getContainer() const;

	protected:
		std::string mContainerName;
		std::map<std::string, std::shared_ptr<T>> mTs;
};

template<class T>
Container<T>::Container(const char* containerName)
	: mContainerName(containerName)
{
}

template<class T>
void Container<T>::addT(std::shared_ptr<T> t)
{
	// does not change the container if already there
	mTs.insert(std::make_pair(t->getName(), t));
}

template<class T>
std::shared_ptr<T> Container<T>::getT(const std::string& n)
{
	auto it = mTs.find(n);
	if(it != mTs.end())
		return it->second;
	else
		return nullptr;
}

template<class T>
const std::string& Container<T>::getName() const
{
	return mContainerName;
}

template<class T>
std::map<std::string, std::shared_ptr<T>>& Container<T>::getContainer()
{
	return mTs;
}

template<class T>
const std::map<std::string, std::shared_ptr<T>>& Container<T>::getContainer() const
{
	return mTs;
}

}

#endif




