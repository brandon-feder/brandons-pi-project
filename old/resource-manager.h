#ifndef __RESOURCE_MANAGER__
#define __RESOURCE_MANAGER__

#include <vector>
#include <mutex>
#include <unordered_set>

using namespace std;

template<class T>
class ResourceManager
{
private:
    vector<T> resources;

    unordered_set<T> free_res;
    mutex free_mutex;

    unordered_set<T> reserved_res;
    mutex reserved_mutex;

public:
    ResourceManager(vector<T> resources_);
    
    size_t n_free();
    T reserve();
    void unreserve(T res);
};

template<class T>
ResourceManager<T>::ResourceManager(vector<T> resources_)
{
    for(T res : resources_)
        free_res.insert(res);
};

template<class T>
size_t ResourceManager<T>::n_free()
{
    free_mutex.lock();
    size_t res = free_res.size();
    free_mutex.unlock();
    return res;
}

template<class T>
T ResourceManager<T>::reserve()
{
    free_mutex.lock();
    if(free_res.size() == 0)
        throw runtime_error("There is nothing free to reserve.\n");
    
    T res = *free_res.begin();
    free_res.erase(res);
    free_mutex.unlock();

    reserved_mutex.lock();
    reserved_res.insert(res);
    reserved_mutex.unlock();

    return res;
}

template<class T>
void ResourceManager<T>::unreserve(T res)
{
    reserved_mutex.lock();
    if(reserved_res.count(res) != 1)
        throw runtime_error("Trying to unreserve resource that is not reserved.");

    reserved_res.erase(res);
    reserved_mutex.unlock();

    free_mutex.lock();
    free_res.insert(res);
    free_mutex.unlock();
}

#endif