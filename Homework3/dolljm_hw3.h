/* 
 * File:   dolljm_hw3.h
 * Copyright (C) 2021 John Doll
 */

#ifndef DOLLJM_HW3_H
#define DOLLJM_HW3_H

#include <unordered_map>
#include <string>

// Use an unordered map to look-up the user-id's and group-id
using IntStrMap = std::unordered_map<int, std::string>;

/**
 * A class to encapsulate methods and instance variables to streamline 
 * implementation. This class has only 1 primary interface method,
 * namely the listFiles() method. This method lists files for a given
 * user while checking for user, group, other permissions for the file
 * and its parents in the directory structure.
 */
class dolljm_hw3 {
public:
    /**
     * The main interface method in this class. This method coordinates
     * all of the activities in this class. 
     * 
     * @param dirFilePath The directory path from where the list of files
     * are to be read and processed.
     * 
     * @param login An optional login ID of the user whose files are to be
     * listed. If this parameter is an empty string, then all the files
     * are listed without performing any entitlements check.
     * 
     * @param permType The type of permission check to be performed. This
     * parameter can be be one of the following strings:
     *     "u": Check only user's permissions to list files.
     *     "g": Check user & group permissions to list files.
     *     "r": Check user, group, and all parent path's permissions.
     */
    void listFiles(const std::string& dirFilePath, const std::string& login,
            const std::string& permType);
    
    /**
     * Method to print out line by line the files and permissions
     * @param perms the permissions associated with the file
     * @param fileUid the userID for this file
     * @param fileGid the groupID for this file
     * @param filePath the path for this file
     */
    void print(std::string perms, const int fileUid, const int fileGid,
               const std::string& filePath);
    
    /**
     * Checks each incoming file to see if given user can access at given 
     * level of permission
     * @param login the user requesting access
     * @param perms the user, group, and other read write and execute permission
     * specifications
     * @param fileGid group level access to file
     * @param pType specifies if we are trying to access through user, group or
     * recursively
     * @param fileUid user level access to file
     * @param path the file path
     * @return returns true if can be accessed with given permissions and user,
     * false otherwise
     */
    bool checkFile(const std::string login, const std::string perms,
                    const int fileGid, const std::string pType, const int 
                    fileUid, std::string path, const std::string fPath);
    
    /**
     * Recursively checks if file is accessible by user all the way up to
     * highest parent file
     * @param login the user who's accessibility we are checking
     * @param parentPath the path that we are testing to see if user has access
     * @param fPath the file that contains the list of all files and permissions
     * @return true if user can access the file and all parents, false otherwise
     */
    bool checkRFile(const std::string login, std::string parentPath, const
                    std::string fPath);
    
    /**
     * Returns the parent path to the given file
     * @param path the file whose parent needs found
     * @return the parent path
     */
    std::string getParentPath(std::string path);
    
protected:
    /** Convenience method to load user-id and login-id from a given
     * password file into the uidName instance variable. This method
     * is essentially a copy of an earlier homework's solution.
     * 
     * \param[in] passFile Path to the file with user information.
     */
    void loadUserIDs(const std::string& passFile = "passwd.txt");

    /**
     * Convenience method to load the group information from a given
     * group file into the gidName and gidUsers maps to ease 
     * processing.  This method is essentially a copy of an earlier 
     * homework's solution.
     * 
     * \param[in] groupFile Path to the file with group information.
     */
    void loadGroupInfo(const std::string& groupFile = "groups.txt");
    
    

private:
    /**
     * The unordered map containing user-id and login name.
     * The entries in this map are populated using the data in the 
     * passwd.txt file. 
     * 
     * Example: {{0, "root"}, {1000, "raodm"}, ...}
     */
    IntStrMap uidName;

    /**
     * The unordered map containing gid and group-names. The entries
     * in this map are populated using the data in the groups.txt file. 
     * Example: {{2, "faculty"}, {5, "admin"}, {6, "theory"}, ...};
     */
    IntStrMap gidName;

    /**
     * The unordered map containing gid and the list of userID's
     * in the group.  The entries in this map are populated using 
     * the data in the groups.txt file. 
     * 
     * Example: {{5, "1001,1002,0"}, {6, "2001,1500,1004,2002", ...}}
     */
    IntStrMap gidUsers;
};

#endif /* DOLLJM_HW3_H */

