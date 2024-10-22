# FIX Message Date Range Filter

This C++ program filters FIX messages in a file based on the `SendingTime` (Tag 52) within a specified date range. It extracts and processes the FIX messages from a given file, filtering them by comparing the `SendingTime` tag with the provided start and end dates.

## Features
- **Date Parsing**: Converts a `SendingTime` string (Tag 52) into a `std::time_t` format to facilitate date comparison.
- **Date Range Filter**: Checks if the message `SendingTime` falls within the specified start and end date range.
- **Tag Value Extraction**: Extracts values of specific FIX tags (e.g., Tag 52 for `SendingTime`).
- **File Processing**: Reads FIX messages from a file, parses them, and displays those that match the date range criteria.

## Usage

1. Place the FIX messages in a file. The program processes files with FIX protocol formatted messages.
2. Specify the date range you want to filter by in the format `YYYYMMDD-HH:MM:SS`.

### Date Format
`SendingTime` in FIX messages should follow the format: `YYYYMMDD-HH:MM:SS`.

### Functionality
- `void filterByDateRange(const std::string& filePath, const std::string& startDate, const std::string& endDate)`: Filters the messages in a file by the `SendingTime` (Tag 52) within the provided date range.

- `std::time_t parseDateTime(const std::string& dateTimeStr)`: Converts a date string into a `std::time_t` object for comparison.

- `bool isDateInRange(const std::string& dateTime, const std::string& startDate, const std::string& endDate)`: Compares if a given date falls within the provided start and end dates.

- `std::string extractTagValue(const std::string& message, const std::string& tag)`: Extracts the value of a specified tag from a FIX message.
