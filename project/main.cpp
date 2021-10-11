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
  logger.push_back(logMessage);
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
    for (int i = 0; i < logger.size(); i++)
    {
      x += logger[i] + "\n";
    }
    // ------- End Critical --------
  }
  else
  {
    // ------- Begin Critical --------
    x += logger[index];
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
};

int Buffer::writeBuffer(int newValue)
{
  // ------- Begin Critical --------
  if (buffer.size() < maxSizeBuffer || maxSizeBuffer == -1)
  {
    buffer.push_back(newValue);
    log.writeLog("Succesfully written to the buffer with value " + to_string(newValue) + ".");
    // ------- End Critical --------
    return 0;
  }

  else
  {
    // ------- Begin Critical --------
    log.writeLog("Failed to writeBuffer, as buffer has reached maximum capacity...");
    // ------- End Critical --------
    return -1;
  }
}

int Buffer::readBuffer()
{
  // ------- Begin Critical --------
  if (!buffer.empty())
  {
    int x = buffer[0];
    buffer.erase(buffer.begin());
    ;
    log.writeLog("Succesfully read the buffer with value " + to_string(x) + ".");
    // ------- End Critical --------
    return x;
  }
  else
  {
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
    buffer.resize(bound);
    maxSizeBuffer = bound;
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
  maxSizeBuffer = -1;
  log.writeLog("Succesfully unbounded the buffer!");
  // ------- End Critical --------
}

int main(int argc, char *argv[])
{
  Logger log = Logger();
  Buffer buffer = Buffer(log);

  buffer.writeBuffer(500);
  buffer.writeBuffer(999);
  buffer.writeBuffer(10);
  cout << buffer.readBuffer();
  buffer.setBoundBuffer(1);
  buffer.writeBuffer(2);
  buffer.writeBuffer(9);
  buffer.setUnboundedBuffer();
  cout << log.readLog(-1);

  return 0;
}