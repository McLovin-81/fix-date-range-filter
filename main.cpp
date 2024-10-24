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


int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0] << " <filePath> <startDate> <endDate>" << std::endl;
        return 1;
    }

    std::string filePath = argv[1];
    std::string startDate = argv[2];
    std::string endDate = argv[3];

    // filterByDateRange("GW_FXNODX-01.FIX.20240621.sfd", "20240621-21:59:40", "20240621-21:59:40");
    filterByDateRange(filePath, startDate, endDate);
    return 0;
}


/**
 * Parse a datetime string into a std::time_t
*/
std::time_t parseDateTime(const std::string& dateTimeStr)
{
    std::tm tm = {};
    std::istringstream ss(dateTimeStr.substr(0, 19));  // Only take up to seconds

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

/**
 * Extract the value of a tag (e.g. 52=) from a FIX message
*/
std::string extractTagValue(const std::string& message, const std::string& tag)
{
    const char SOH = '\x01';
    std::string searchTag = tag + "=";
    size_t tagPos = message.find(searchTag);
    if (tagPos == std::string::npos)
    {
        return "";
    }

    size_t valueStart = tagPos + searchTag.length();
    size_t valueEnd = message.find(SOH, valueStart);

    return (valueEnd == std::string::npos) ? "" : message.substr(valueStart, valueEnd - valueStart);
}

/**
 * Filters FIX messages in a file by 'sending time (Tag 52)' within a date range
*/
void filterByDateRange(const std::string& filePath, const std::string& startDate, const std::string& endDate)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Unable to open file" << std::endl;
        return;
    }

    // Read entire file into a string
    std::string buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    size_t pos = 0;
    const char SOH = '\x01';

    while (pos < buffer.size())
    {
        // Find the start of a message (usually starts with 8=FIX) // 
        size_t startPos = buffer.find("8=FIX", pos);
        if (startPos == std::string::npos)
        {
            break; // No more messages found
        }

        // Find the end of the message (assuming it ends with tag "10=" followed by 3 digits and SOH) //
        size_t checksumPos = buffer.find("10=", startPos);
        if (checksumPos == std::string::npos || checksumPos + 7 >= buffer.size())
        {
            std::cerr << "Warning: Incomplete or malformed message starting at position " << startPos << std::endl;
            pos = startPos + 1; // Move to the next character and continue
            continue;
        }

        // Ensure that checksum ends with SOH
        size_t checksumEnd = checksumPos + 3 + 3; // 3 + 3 = "10=" + 3 digits for checksum
        if (buffer[checksumEnd] != SOH)
        {
            std::cerr << "Warning: Missing SOH after checksum at message starting at position " << startPos << std::endl;
            pos = startPos + 1; // Continue searching
            continue;
        }

        // Extract the full message //
        std::string message = buffer.substr(startPos, checksumEnd + 1);

        // Extract sending time (tag 52) //
        std::string sendingTime = extractTagValue(message, "52");
        
        if (sendingTime.empty())
        {
            std::cerr << "Warning: Message at position " << startPos << " missing Tag 52 (Sending Time)" << std::endl;
        }
        else
        {
            // Check if the sending time is within the provided date range
            if (isDateInRange(sendingTime, startDate, endDate))
            {
                std::cout << "sendingTime: " << sendingTime << std::endl;
                std::cout << message << std::endl;
            }
        }

        // Move to the next message
        pos  = checksumEnd + 1;
    }   
}