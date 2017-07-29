#ifndef PERSISTENCE_DATASTREAM_H
#define PERSISTENCE_DATASTREAM_H

#include <boost/variant.hpp>

#include <algorithm>
#include <memory>
#include <mutex>
#include <vector>

namespace persistence
{
  template <class T>
  class DataStreamObserver
  {
  public:
    virtual ~DataStreamObserver() {}

    virtual void addItem(const T& item) = 0;
    virtual void removeItem(int id) = 0;
    virtual void clear() = 0;
  };

  template <class T>
  class VectorDataStreamObserver : public DataStreamObserver<T>
  {
  public:
    const std::vector<T>& items() const { return _dataItems; }

    virtual void addItem(const T& item) override { _dataItems.push_back(item); }
    virtual void removeItem(int id) override
    {
      _dataItems.erase(std::remove_if(_dataItems.begin(), _dataItems.end(),
                                      [id](const T& item) { return item.id() == id; }), _dataItems.end());
    }
    virtual void clear() override { _dataItems.clear(); }

  private:
    std::vector<T> _dataItems;
  };

  /**
   * @brief Writable backend for data stream
   */
  template <class T>
  class DataStream
  {
  public:
    DataStream(int streamId, DataStreamObserver<T> *observer)
        : _streamId(streamId), _observer(observer)
    {}

    /**
     * @brief integrateChanges integrates all outstanding changes
     *
     * Calling this method will inform the observer of all the changes that have happened. Be sure to call this method
     * on the main thread. This method is usually called by the ResultIntegrator class.
     */
    void integrateChanges()
    {
      std::lock_guard<std::mutex> lock(_pendingOperationsMutex);
      if (_observer)
      {
        for (auto& operation : _pendingOperations)
          boost::apply_visitor([this](const auto& op) { return this->integrate(op); }, operation);
      }
      _pendingOperations.clear();
    }

    // Interface for backend
    void addItems(std::vector<T> newItems)
    {
      std::lock_guard<std::mutex> lock(_pendingOperationsMutex);
      _pendingOperations.push_back(ItemsAdded{std::move(newItems)});
    }
    void removeItems(std::vector<int> itemsToRemove)
    {
      std::lock_guard<std::mutex> lock(_pendingOperationsMutex);
      _pendingOperations.push_back(ItemsRemoved{std::move(itemsToRemove)});
    }
    void clear()
    {
      std::lock_guard<std::mutex> lock(_pendingOperationsMutex);
      _pendingOperations.push_back(Cleared());
    }

    //! Returns the unique ID of this stream
    int streamId() const { return _streamId; }
    //! Returns true if there is still an observer listening on this stream
    bool isValid() const { return _observer != nullptr; }
    //! Dissociates the stream from the observer.
    void disconnect() { _observer = nullptr; }

  private:
    struct ItemsAdded { std::vector<T> newItems; };
    struct ItemsRemoved { std::vector<int> removedItems; };
    struct Cleared {};
    typedef boost::variant<ItemsAdded, ItemsRemoved, Cleared> StreamOperation;

    // Private help methods called by the integrateChanges() method. _observer is guaranteed to be not null when these
    // functions are called
    void integrate(const ItemsAdded& op) { for (auto& added : op.newItems) _observer->addItem(added); }
    void integrate(const ItemsRemoved &op) { for (auto removed : op.removedItems) _observer->removeItem(removed); }
    void integrate(const Cleared&) { _observer->clear(); }

    int _streamId;
    DataStreamObserver<T>* _observer;

    std::mutex _pendingOperationsMutex;
    std::vector<StreamOperation> _pendingOperations;
  };

  template <class T>
  class UniqueDataStreamHandle
  {
  public:
    UniqueDataStreamHandle(std::shared_ptr<DataStream<T>> dataStream)
        : _dataStream(dataStream)
    {}

    UniqueDataStreamHandle(const UniqueDataStreamHandle<T>& that) = delete;
    UniqueDataStreamHandle<T>& operator=(const UniqueDataStreamHandle<T>& that) = delete;

    UniqueDataStreamHandle(UniqueDataStreamHandle<T>&& that) = default;
    UniqueDataStreamHandle<T>& operator=(UniqueDataStreamHandle<T>&& that) = default;
    ~UniqueDataStreamHandle() { _dataStream->disconnect(); }

  private:
    std::shared_ptr<DataStream<T>> _dataStream;
  };

}

#endif // PERSISTENCE_DATASTREAM_H