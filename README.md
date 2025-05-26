# pkgsaurus: A Customized Package Manager for xv6 
![logo](https://github.com/user-attachments/assets/c0e835fa-03d4-457a-8fe1-efe071ce3314)

 

## Team Members

 Samer Ahmed Elhossany (221001697)
 
 Alzahraa Saeed Gwyly (221000968)
 
Youssef Ehab Abbas (221000189)

Mohamed Hassan Ahmed (221001705)

Mustafa Adel Abdullah (221002165)

# special thanks for: 
## Dr Eman Kamel 

## Eng Gannat Elsayed Sharaf Elden

## Eng Mohamed Samir Mourad




# 1. Introduction
The Pkgsaurus project is a customized package manager designed specifically for the xv6 operating system. The primary goal of Pkgsaurus is to automate the process of installing, updating, removing, and managing software packages within the xv6 environment, addressing a critical gap in xv6’s ecosystem.

## 1.1 What is a Package Manager?
A package manager is a command-line or graphical tool that automates the installation, updating, and removal of software packages. It ensures dependency resolution, version control, and efficient software management. Package managers can be categorized into two types:
-  OS-based package managers (e.g., APT, YUM, Homebrew, Chocolatey) – Manage system-level packages.
- Language-based package managers (e.g., pip, npm) Handle libraries and modules for specific programming languages.
Since xv6 lacks a native package manager, Pkgsaurus fills this gap by providing essential package management functionalities.

# 2. Project Overview
Pkgsaurus is a lightweight yet powerful package manager tailored for xv6. Its key features include:
- Installation & Removal of packages.
- Updating & Upgrading packages to newer versions.
- Searching for available packages.
- Documentation for user guidance.
- CRC Verification to ensure file integrity.
- Custom Commands (e.g., `draw`) for additional functionalities.

## 2.1 Why xv6?
xv6 is a simple, Unix-like teaching OS used in academic courses. However, it lacks a built-in package manager, making software management cumbersome. Pkgsaurus solves this by introducing a structured way to handle packages in xv6.



# 3. Functionalities
Pkgsaurus offers a variety of functions to streamline package management in xv6.

## 3.1 Documentation Function
- Command: `pkgsaurus`
- Displays a help menu with usage instructions.
- Ensures users understand available commands and syntax.

## 3.2 Draw Function
- Command: `pkgsaurus draw <drawing>`
- A fun, experimental feature that allows users to generate ASCII-based drawings in the terminal.

## 3.3 Initialization Function
- Sets up the necessary directories and files:
  - `RegistryFile.txt` (tracks installed packages).
  - `pkgsaurusList.txt` (lists available packages).
  - `Upgradable list` (tracks packages with available updates).
  - `server/` (hosts package files).
  - `Pkgsaurus_packages/` (stores installed packages).
- Creates sample packages (`calculator`, `count10`, `testing`) for demonstration.

## 3.4 Binary File Creation for xv6
A unique challenge was embedding executable binaries in xv6. The process involved:
1. Writing C code and compiling it inside xv6 (added to the Makefile).
2. Converting the executable to a header file using `xxd -i <executable> > executable.h`.
3. Embedding the binary data in a C program to regenerate the executable at runtime.

## 3.5 Install Function
- Command: `pkgsaurus install <package>`
- Steps:
  1. Checks `pkgsaurusList` for package availability.
  2. Updates `RegistryFile.txt` with the new package.
  3. Copies the package to `Pkgsaurus_packages/`.

## 3.6 Search Function
- Command: `pkgsaurus search <package>`
- Searches `pkgsaurusList.txt` for matching packages.

## 3.7 Remove Function
- Command: `pkgsaurus remove <package>`
- Steps:
  1. Deletes the package from `Pkgsaurus_packages/`.
  2. Updates `RegistryFile.txt` by removing the entry.

## 3.8 Update & Upgrade Functions
- Update: `pkgsaurus update`  
  Compares installed packages with server versions and populates the `Upgradable list`.
- Upgrade: `pkgsaurus upgrade`  
  Installs newer versions from the `Upgradable list` using the `install` function.

## 3.9 List Function
- Command: `pkgsaurus list`  
  Displays all installed packages from `RegistryFile.txt`.

## 3.10 CRC Verification
- Ensures file integrity by:
  1. Calculating a checksum for every 128 bytes of a file (`write_crc`).
  2. Storing checksums in `.cksum` files (e.g., `ls.cksum`).
  3. Verifying files by recalculating and comparing checksums (`verify_crc`).


# 4. Implementation Challenges
1. Binary Execution in xv6:  
   - Required converting executables to header files and embedding them in C programs.
2. File Management:  
   - Maintaining consistency between `RegistryFile.txt`, `pkgsaurusList.txt`, and installed packages.
3. CRC Implementation:  
   - Ensuring accurate checksum generation and verification for file integrity.

# 5. Future Enhancements
- Dependency Resolution: Automatically handle package dependencies.
- Network Support: Fetch packages from a remote repository.
- Graphical Interface: A user-friendly TUI for easier interaction.
- Extended Commands: More utility functions (e.g., `pkgsaurus info` for package details).

 # 6. Conclusion

Pkgsaurus successfully introduces a functional package manager to xv6, simplifying software management in an otherwise minimal OS. As Craig Reucassel said, "It's hardware that makes a machine fast. It's software that makes a fast machine slow." Pkgsaurus ensures that software management in xv6 remains efficient and user-friendly.

Images for the program execution: 








![Screenshot 2025-05-26 020448](https://github.com/user-attachments/assets/7bbc7850-b011-4680-81d4-ce548f93e30d)

![Screenshot 2025-05-26 020649](https://github.com/user-attachments/assets/ae1e19df-79fa-48f5-9673-76c1a0fff661)

![Screenshot 2025-05-26 020905](https://github.com/user-attachments/assets/93edc9c0-2250-402f-bae0-15d0c4fc1ab9)

![Screenshot 2025-05-26 021144](https://github.com/user-attachments/assets/70b598d1-d3e6-4050-a3c7-f42d18924d61)


![Screenshot 2025-05-26 021248](https://github.com/user-attachments/assets/954eceb5-4d43-4479-8b4c-07eb2c95334e)

![Screenshot 2025-05-26 021509](https://github.com/user-attachments/assets/e8e3f657-1f62-4280-930d-af4d745aea30)

![Screenshot 2025-05-26 021805](https://github.com/user-attachments/assets/e470a7be-3cc2-4107-8d81-5f9e3c73c508)
