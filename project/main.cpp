/**
  * Assignment: synchronization
  * Operating Systems
  */

/**
  Hint: F2 (or Control-klik) on a functionname to jump to the definition
  Hint: Ctrl-space to auto complete a functionname/variable.
  */

// function/class definitions you are going to use
#include <algorithm>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <sstream>

// although it is good habit, you don't have to type 'std::' before many objects by including this line
using namespace std;

mutex lockBuffer;
mutex lockLogger;
mutex lockBounds;

class Logger
{
private:
  vector<string> logger;

public:
  void writeLog(string logMessage);
  string readLog(int index);
};

void Logger::writeLog(string logMessage)
{
  // ------- Begin Critical --------
  lockLogger.lock();
  logger.push_back(logMessage);
  lockLogger.unlock();
  // ------- End Critical --------
}

/*
  @returns logMessage at index. If index is -1 returns all items.
*/
string Logger::readLog(int index)
{
  std::thread::id this_id = std::this_thread::get_id();
  std::ostringstream ss;
  ss << this_id;
  string x = "[" + ss.str() + "] ";
  if (index == -1)
  {

    // ------- Begin Critical --------
    lockLogger.lock();
    for (int i = 0; i < logger.size(); i++)
    {
      x += logger[i] + "\n";
    }
    lockLogger.unlock();
    // ------- End Critical --------
  }
  else
  {
    // ------- Begin Critical --------
    lockLogger.lock();
    x += logger[index];
    lockLogger.unlock();
    // ------- End Critical --------
  }
  return x;
}

class Buffer
{
private:
  vector<int> buffer;
  int maxSizeBuffer;
  Logger log;

public:
  Buffer(Logger &logger)
  {
    log = logger;
    maxSizeBuffer = -1;
  }
  int writeBuffer(int newValue);
  int readBuffer();
  int setBoundBuffer(int bound);
  void setUnboundedBuffer();
  Logger getLogger();
  int size();
  int getBound();
};

int Buffer::writeBuffer(int newValue)
{
  // ------- Begin Critical --------
  lockBuffer.lock();
  // cout << "Writing to buffer with size : " + to_string(buffer.size()) << endl;
  if (buffer.size() < maxSizeBuffer || maxSizeBuffer == -1)
  {
    // cout << "Hi from if" << endl;
    buffer.push_back(newValue);
    log.writeLog("Succesfully written to the buffer with value " + to_string(newValue) + ".");
    // ------- End Critical --------
    lockBuffer.unlock();
    return 0;
  }

  else
  {
    // cout << "Hi from else" << endl << "Because " + to_string(buffer.size()) + " > " + to_string(maxSizeBuffer) + " or " + to_string(maxSizeBuffer) + " != -1 " << endl;
    lockBuffer.unlock();
    // ------- Begin Critical --------
    log.writeLog("Failed to writeBuffer, as buffer has reached maximum capacity...");
    // ------- End Critical --------
    return -1;
  }
}

int Buffer::readBuffer()
{
  // ------- Begin Critical --------
  lockBuffer.lock();
  if (!buffer.empty())
  {
    int x = buffer[0];
    buffer.erase(buffer.begin());
    ;
    log.writeLog("Succesfully read the buffer with value " + to_string(x) + ".");
    lockBuffer.unlock();
    // ------- End Critical --------
    return x;
  }
  else
  {
    lockBuffer.unlock();
    // ------- Begin Critical --------
    log.writeLog("Failed to readBuffer, as buffer seems to be empty...");
    // ------- End Critical --------
    return -1;
  }
}

int Buffer::setBoundBuffer(int bound)
{
  if (bound >= 0)
  {
    // ------- Begin Critical --------
    lockBuffer.lock();
    if (buffer.size() > bound)
    {
      buffer.resize(bound);
    }
    lockBuffer.unlock();
    lockBounds.lock();
    maxSizeBuffer = bound;
    lockBounds.unlock();
    log.writeLog("Succesfully set buffer's bound to " + to_string(bound));
    // ------- End Critical --------
    return 0;
  }
  else
  {
    // ------- Begin Critical --------
    log.writeLog("Failed to set buffer, as buffer cannot be negative " + to_string(bound) + " is an invalid value.");
    // ------- End Critical --------
    return -1;
  }
}

void Buffer::setUnboundedBuffer()
{
  // ------- Begin Critical --------
  lockBounds.lock();
  maxSizeBuffer = -1;
  lockBounds.unlock();
  log.writeLog("Succesfully unbounded the buffer!");
  // ------- End Critical --------
}

Logger Buffer::getLogger()
{
  return log;
}

int Buffer::size()
{
  return buffer.size();
}

int Buffer::getBound()
{
  return maxSizeBuffer;
}

void addRandomInt(Buffer &buffer)
{
  buffer.writeBuffer(rand() % 100);
}

void testAddingItems(Buffer &buffer, int toWrite)
{

  vector<thread> ThreadVector;

  for (int i = 0; i < toWrite; i++)
  {
    ThreadVector.emplace_back([&]()
                              { addRandomInt(buffer); }); // Pass by reference here, make sure the object lifetime is correct
  }
  for (auto &t : ThreadVector)
  {
    t.join();
  }

  // cout << buffer.getLogger().readLog(-1);
}

void readItem(Buffer &buffer)
{
  cout << buffer.readBuffer() << endl;
}

void testReadingItems(Buffer &buffer, int toRead)
{
  vector<thread> ThreadVector;

  for (int i = 0; i < toRead; i++)
  {
    ThreadVector.emplace_back([&]()
                              { readItem(buffer); }); // Pass by reference here, make sure the object lifetime is correct
  }
  for (auto &t : ThreadVector)
  {
    t.join();
  }

  cout << buffer.getLogger().readLog(-1);
}

void testBound(Buffer &buffer, int newBound)
{
  // cout << "Before testing buffer size is : " + to_string(buffer.size()) << " and boud is set to : " + to_string(buffer.getBound()) << endl;
  buffer.setBoundBuffer(newBound);

  for (int i = 0; i < newBound + 1; i++)
  {
    addRandomInt(buffer);
  }
  cout << buffer.getLogger().readLog(-1);
}

void testUnbounding(Buffer &buffer, int bound)
{
  buffer.setUnboundedBuffer();

  for (int i = 0; i < bound + 1; i++)
  {
    addRandomInt(buffer);
  }

  cout << buffer.getLogger().readLog(-1);
}

int main(int argc, char *argv[])
{
  Logger log = Logger();
  Buffer buffer = Buffer(log);

  int noItems = 10;

  //  Adding items
  testAddingItems(buffer, noItems);
  cout << "Done adding items..." << endl;
  //  Reading items
  testReadingItems(buffer, noItems);
  //  Testing bound
  cout << "-------------Testing Bounds--------------" << endl;
  int bound = 2;
  Logger log2 = Logger();
  Buffer buffer2 = Buffer(log2);
  testBound(buffer2, bound);

  cout << "--------------Testing Unbounding---------------" << endl;
  testUnbounding(buffer2, bound);

  cout << "All threads finished..." << endl;

  return 0;
}