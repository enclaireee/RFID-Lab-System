# RFID Lab System

A comprehensive C++ application for managing laboratory access control using RFID card scanning. This system tracks user entry/exit times, maintains attendance logs, and provides detailed reporting capabilities.

## Features

### 🔐 User Management
- Add new users with role-based access (Student, Staff, Faculty)
- Input validation for user IDs and names
- Duplicate user prevention
- User status tracking (IN/OUT)

### 📱 RFID Scanning
- Real-time card scanning simulation
- Automatic status toggling (IN ↔ OUT)
- Timestamped entry/exit logging
- User verification and error handling

### 📊 Reporting & Analytics
- Search logs by specific user ID
- Display all scan logs (chronologically sorted)
- Current user status overview
- Daily attendance reports with scan counts
- Export capabilities (Binary & JSON formats)

### 💾 Data Persistence
- Automatic data saving to binary files
- JSON export for external integrations
- Data directory management
- Session recovery on system restart

## System Requirements

- C++ compiler with C++11 support or higher
- Standard C++ libraries
- POSIX-compatible system (for directory operations)

## Installation & Setup

### 1. Compilation
```bash
# Compile all source files
g++ -std=c++11 -o program main.cpp RFIDSystem.cpp

# Alternative with all files explicitly
g++ -std=c++11 main.cpp RFIDSystem.cpp -o program
```

### 2. Directory Structure
```
project_folder/
├── main.cpp
├── RFIDSystem.cpp
├── RFIDSystem.h
├── User.h
├── ScanLog.h
├── rfid_system (executable)
└── data/
    ├── daily_logs.bin
    └── daily_logs.json
```

### 3. First Run
```bash
./program
```

The system will automatically:
- Create a `data/` directory
- Initialize empty log files
- Display the main menu

## Usage Guide

### Main Menu Options

```
========== RFID LAB SYSTEM ==========
1. Add New User
2. Scan Card
3. Search Logs
4. Display All Logs
5. Display User Status
6. Display Daily Report
7. Save Data to Binary File
8. Export Data to JSON
9. Clear Daily Logs
0. Exit System
===============================================
```

### 1. Adding Users

**Requirements:**
- **User ID**: 3-10 alphanumeric characters (unique)
- **Name**: 2-50 characters (letters, spaces, dots, hyphens only)
- **Role**: Student, Staff, or Faculty

**Example:**
```
User ID: STU001
Name: John Doe
Role: Student
```

### 2. RFID Card Scanning

Enter a valid User ID to simulate card scanning:
- First scan: User status changes to "IN"
- Second scan: User status changes to "OUT"
- Each scan creates a timestamped log entry

### 3. Log Management

**Search Logs**: Find all scan history for a specific user
**Display All Logs**: View complete chronological scan history
**Daily Report**: Summary with total scans per user

### 4. Data Export

**Binary Format**: Efficient storage for system recovery
**JSON Format**: Human-readable export for external systems

## File Formats

### Binary File Structure
```
- Log count (size_t)
- For each log:
  - User ID length + data
  - User name length + data
  - Action length + data
  - Timestamp (time_t)
```

### JSON Export Format
```json
{
  "daily_logs": [
    {
      "user_id": "STU001",
      "user_name": "John Doe",
      "action": "IN",
      "timestamp": "2025-06-04 14:30:15",
      "unix_timestamp": 1717507815
    }
  ],
  "total_scans": 1,
  "export_time": "2025-06-04 14:35:22"
}
```

## Error Handling

The system includes comprehensive error handling for:

- **Invalid User IDs**: Non-alphanumeric or wrong length
- **Duplicate Users**: Prevents ID conflicts
- **Empty Inputs**: Validates all required fields
- **File Operations**: Handles read/write errors gracefully
- **Memory Management**: Automatic cleanup and recovery

## Best Practices

### Daily Operations
1. Start system at beginning of day
2. Add new users as needed
3. Use "Scan Card" for all entry/exit tracking
4. Review daily reports before closing
5. System auto-saves on exit

### Data Management
- Export to JSON regularly for backups
- Clear daily logs at end of each day/period
- Monitor data directory for disk space

### User Management
- Use consistent ID naming conventions
- Verify user information before confirmation
- Regularly review user status for accuracy

## Troubleshooting

### Common Issues

**"User not found" error**
- Verify User ID exists in system
- Check for typos in User ID
- Use "Display User Status" to see all users

**File permission errors**
- Ensure write permissions in project directory
- Check if `data/` directory exists
- Run with appropriate user privileges

**Compilation errors**
- Verify all source files are present
- Check C++11 compiler support
- Ensure proper include paths

### Data Recovery

If binary files become corrupted:
1. Delete `data/daily_logs.bin`
2. Restart system (creates fresh files)
3. Re-add users manually
4. Import from JSON backup if available

## Technical Specifications

### Data Structures
- **User**: ID, Name, Role
- **ScanLog**: UserID, UserName, Action, Timestamp
- **System**: User vector, Log vector, Status map

### Performance
- **Memory**: Scales with user count and daily scans
- **Storage**: Binary format for efficiency
- **Speed**: O(n) search operations, O(n log n) sorting

### Security Considerations
- No encryption (add if needed for sensitive environments)
- File-based storage (consider database for high-volume usage)
- Input validation prevents basic injection attacks

## Support & Maintenance

### Regular Maintenance
- Weekly data exports
- Monthly log archival
- Periodic system restarts
- User database cleanup

### System Monitoring
- Check data file sizes
- Monitor scan patterns for anomalies
- Verify user status accuracy
- Review error logs

## License

This project is provided as-is for educational and internal use purposes.

---

**Version**: 1.0
**Last Updated**: June 2025
**Compatibility**: C++11 and higher
