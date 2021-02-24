/* 
 * File:   raodm_hw3.h
 * Author: raodm
 *
 * Copyright (C) 2021 raodm@miamioh.edu
 */

#ifndef RAODM_HW3_H
#define RAODM_HW3_H

#include <unordered_map>
#include <string>

// Use an unordered map to look-up the user-id's and group-id
using IntStrMap = std::unordered_map<int, std::string>;

class raodm_hw3 {
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
    
protected:
    /**
     * Process data from a given text file and print files for which
     * the given user has read, write, or execute permissions.
     * 
     * @param filePath The path to the file from where the data for the files
     * is to be read.
     * 
     * @param login The user's login ID for which the files are to be listed. If this
     * is an empty string, then all the files are listed.
     * 
     * @param uid The user ID corresponding to the login ID.
     * 
     * @param chkType The type of checks to be performed as in:
     *    'u': Only user permissions are to be used to decide access.
     *    'g': User & group permissions are to be used to decide access.
     *    'r': User, group, & parent folder permissions are to be used.
     */
    void listFiles(const std::string& filePath, const std::string& login,
            const int uid, const std::string chkType);

    /** Convenience method to load user-id and login-id from a given
     * password file into the uidName instance variable.
     * 
     * \param[in] passFile Path to the file with user information.
     */
    void loadUserIDs(const std::string& passFile = "passwd.txt");

    /**
     * Convenience method to load the group information from a given
     * group file into the gidName and gidUsers maps to ease 
     * processing.
     * 
     * \param[in] groupFile Path to the file with group information.
     */
    void loadGroupInfo(const std::string& groupFile = "groups.txt");

    /**
     * Helper method to return the UID for a given login ID. If
     * the logIn ID is empty then this method returns zero.
     * 
     * @param login The login ID for which the uid value is to be
     * returned by searching the uidName map.
     * 
     * @return The uid for the given login id.
     */
    int getUid(const std::string& login) const;  

    /**
     * Helper method to print information about a given file in
     * a nice/required tab-delimited format.
     * 
     * @param filePerms The permissions for the file.
     * 
     * @param fileUid The uid for the file. This is converted to
     * login ID for display.
     * 
     * @param fileGid The gid for the file. This is converted to the
     * name of the group for printing.
     * 
     * @param filePath The path to the file.
     */
    void printFile(std::string filePerms = "rwxr-x---", 
            const int fileUid = 1000, const int fileGid = 5,
            const std::string& filePath = "/lib/admin/xset.sh") const;

    /**
     * Refactored utility method to determine if the parent directory
     * of a given directory has necessary permissions for the given path.
     * If the path is accessible, then this method adds an entry to the
     * accPaths unordered map and returns true. If the parent path does
     * not have access then this method returns false.
     *    
     * @param accPaths An unordered map with the current set of path entries
     * that are currently accessible to the given user.
     * 
     * @param path The path to the file whose parent directory needs to be
     * checked for access permissions.
     * 
     * @return This method returns true if the parent directory is accessible
     * to the user. Otherwise this method returns false.
     */
    bool havePerms(std::unordered_map<std::string, bool>& accPaths,
        const int fileUid, const int fileGid, const std::string& path, 
        const std::string& perms, const int uid, 
        const std::string& chkType) const;

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

#endif /* RAODM_HW3_H */

