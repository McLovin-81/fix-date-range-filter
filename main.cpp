#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>
#include <iomanip>



void filterByDateRange(const std::string& filePath, const std::string& startDate, const std::string& endDate);
std::time_t parseDateTime(const std::string& dateTimeStr);
bool isDateInRange(const std::string& dateTime, const std::string& startDate, const std::string& endDate);
std::string extractTagValue(const std::string& message, const std::string& tag);


int main()
{
/*     std::cout << "Test Case 1: " << std::endl;
    std::cout << isDateInRange("20240620-22:40:09:23", "20240620-22:00:00:00", "20240621-00:00:00:00") << std::endl; // Expected: 1 (true)
    std::cout << "Test Case 2: " << std::endl;
    std::cout << isDateInRange("20240619-22:40:09", "20240620-22:00:00", "20240621-00:00:00") << std::endl; // Expected: 0 (false)
    std::cout << "Test Case 3: " << std::endl;
    std::cout << isDateInRange("20240622-22:40:09", "20240620-22:00:00", "20240621-00:00:00") << std::endl; // Expected: 0 (false)
    std::cout << "Test Case 4: " << std::endl;
    std::cout << isDateInRange("20240620-22:00:00", "20240620-22:00:00", "20240621-00:00:00") << std::endl; // Expected: 1 (true)
    std::cout << "Test Case 5: " << std::endl;
    std::cout << isDateInRange("20240621-00:00:00", "20240620-22:00:00", "20240621-00:00:00") << std::endl; // Expected: 1 (true)
    std::cout << "Test Case 6: " << std::endl;
    std::cout << isDateInRange("20240620-21:00:00", "20240620-22:00:00", "20240621-00:00:00") << std::endl; // Expected: 0 (false)
    std::cout << "Test Case 7: " << std::endl;
    std::cout << isDateInRange("20240621-01:00:00", "20240620-22:00:00", "20240621-00:00:00") << std::endl; // Expected: 0 (false)
    std::cout << "Test Case 8: " << std::endl;
    std::cout << isDateInRange("20240620-23:00:00", "20240620-22:00:00", "20240621-00:00:00") << std::endl; // Expected: 1 (true)
 */

    filterByDateRange("test.sfd", "20240620-22:40:08", "20240620-22:40:37");

    return 0;
}



std::time_t parseDateTime(const std::string& dateTimeStr)
{
    std::tm tm = {};
    std::istringstream ss(dateTimeStr.substr(0, 19));  // Parse up to seconds

    ss >> std::get_time(&tm, "%Y%m%d-%H:%M:%S");
    return std::mktime(&tm);
}

bool isDateInRange(const std::string& dateTime, const std::string& startDate, const std::string& endDate)
{
    std::time_t dateTimeT = parseDateTime(dateTime);
    std::time_t startDateT = parseDateTime(startDate);
    std::time_t endDateT = parseDateTime(endDate);

    return dateTimeT >= startDateT && dateTimeT <= endDateT;
}

std::string extractTagValue(const std::string& message, const std::string& tag)
{
    const char SOH = '\x01';
    std::string searchTag = tag + "=";
    size_t pos = message.find(searchTag);
    if (pos == std::string::npos)
    {
        return "";
    }

    size_t valueStart = pos + searchTag.length();
    size_t valueEnd = message.find(SOH, valueStart);
    if (valueEnd == std::string::npos)
    {
        valueEnd = message.length(); // NOTE: No needed there should be always a SOH in the end
    }
    return message.substr(valueStart, valueEnd - valueStart);
}

void filterByDateRange(const std::string& filePath, const std::string& startDate, const std::string& endDate)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Unable to open file" << std::endl;
        return;
    }

    // Read the entire file into a string buffer
    std::string buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    size_t pos = 0;
    const char SOH = '\x01';

    while (pos < buffer.size())
    {
        // Find the start of the message // 
        size_t startPos = buffer.find("8=FIX", pos);
        if (startPos == std::string::npos)
        {
            break; // No more messages
        }

        // Find the end of the message (assuming it ends with "10=" followed by 3 digits and SOH) //
        size_t endPos = buffer.find(SOH + "10=", startPos);
        if (endPos == std::string::npos)
        {
            std::cerr << "Warning: Incomplete or malformed message starting at position " << startPos << std::endl;
            break; // Cannot process incomplete message
        }
        endPos = buffer.find(SOH, endPos + 1);  // Move to the end of checksum tag
        if (endPos == std::string::npos)
        {
            std::cerr << "Warning: Message at position " << startPos << " missing terminating SOH character after checksum" << std::endl;
            break;
        }

        // Extract the full message //
        std::string message = buffer.substr(startPos, endPos - startPos + 1);
        // Extract sending time (tag 52) //
        std::string sendingTime = extractTagValue(message, "52");
        std::cout << "sendingTime var: " << sendingTime << std::endl;
        if (sendingTime.empty())
        {
            std::cerr << "Warning: Message at position " << startPos << " missing Tag 52 (Sending Time)" << std::endl;
        }
        else
        {
            if (isDateInRange(sendingTime, startDate, endDate))
            {
                std::cout << message << std::endl;
            }
        }

        // Move to the next message
        pos = endPos + 1;
    }   
}