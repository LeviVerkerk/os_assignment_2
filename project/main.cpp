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

mutex lockBuffer;
mutex lockLogger;
mutex lockBounds;

// although it is good habit, you don't have to type 'std::' before many objects by including this line
using namespace std;

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
  string x = "";
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
  Buffer(Logger& logger)
  {
    log = logger;
    maxSizeBuffer = -1;
  }
  int writeBuffer(int newValue);
  int readBuffer();
  int setBoundBuffer(int bound);
  void setUnboundedBuffer();
  Logger getLogger();
};

int Buffer::writeBuffer(int newValue)
{
  // ------- Begin Critical --------
  lockBuffer.lock();
  if (buffer.size() < maxSizeBuffer || maxSizeBuffer == -1)
  {
    buffer.push_back(newValue);
    log.writeLog("Succesfully written to the buffer with value " + to_string(newValue) + ".");
    // ------- End Critical --------
    lockBuffer.unlock();
    return 0;
  }


  else
  {
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
    buffer.resize(bound);
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

Logger Buffer::getLogger() {
  return log;
}

int main(int argc, char *argv[])
{
  Logger log = Logger();
  Buffer buffer = Buffer(log);

  buffer.writeBuffer(500);
  buffer.writeBuffer(999);
  buffer.writeBuffer(10);
  buffer.setBoundBuffer(1);
  buffer.writeBuffer(2);
  buffer.writeBuffer(9);
  buffer.setUnboundedBuffer();
  buffer.writeBuffer(2);
  buffer.writeBuffer(9);
  for (int i = 0; i < 3; i++) {
    cout << "[BUFFER READ] " + to_string(buffer.readBuffer()) + "\n";
  }
  
  cout << buffer.getLogger().readLog(-1);


  return 0;
}