# Requirements Document

## Introduction

This document specifies the requirements for a car salon database management system. The system enables users to create, view, search, modify, and export automobile inventory data stored in a PostgreSQL database. The application provides a graphical user interface (GUI) built with Qt framework, accessible either as a standalone executable or through a web browser via VNC, and runs in a Docker containerized environment.

## Glossary

- **AutoSalon System**: The complete software application for managing car salon inventory
- **Car Record**: A database entry containing automobile information (brand, manufacturer, release date, mileage, price)
- **PostgreSQL Database**: The relational database management system used for data persistence
- **Qt GUI**: The graphical user interface framework used for the application frontend
- **Docker Container**: The isolated runtime environment for the application and database
- **Export File**: A text file containing formatted car data (brand, mileage, price)
- **User**: A person interacting with the AutoSalon System through the GUI

## Requirements

### Requirement 1

**User Story:** As a user, I want to initialize the database with the proper schema, so that I can store car records in a structured format.

#### Acceptance Criteria

1. WHEN the AutoSalon System starts THEN the system SHALL establish a connection to the PostgreSQL Database running on port 5432
2. WHEN the database connection is established THEN the system SHALL create a table with columns for brand, manufacturer, release date, mileage, and price if the table does not exist
3. IF the database connection fails THEN the system SHALL display an error message to the User and prevent further operations
4. WHEN the table is created THEN the system SHALL use appropriate data types for each column (text for strings, date for release date, integer for mileage and price)

### Requirement 2

**User Story:** As a user, I want to view all car records in the database, so that I can see the complete inventory.

#### Acceptance Criteria

1. WHEN the User opens the application THEN the AutoSalon System SHALL display all Car Records in a table view
2. WHEN the User clicks the refresh button THEN the system SHALL reload all Car Records from the PostgreSQL Database and update the display
3. WHEN displaying Car Records THEN the system SHALL show all fields (brand, manufacturer, release date, mileage, price) for each record
4. WHEN the database contains no records THEN the system SHALL display an empty table with column headers

### Requirement 3

**User Story:** As a user, I want to search for cars by price, so that I can find vehicles within my budget.

#### Acceptance Criteria

1. WHEN the User enters a maximum price value and initiates a search THEN the AutoSalon System SHALL display only Car Records where the price is less than or equal to the specified value
2. WHEN the User provides a non-numeric price value THEN the system SHALL reject the input and display an error message
3. WHEN no Car Records match the price criteria THEN the system SHALL display an empty table with a message indicating no results found
4. WHEN the User clears the filter THEN the system SHALL restore the display to show all Car Records

### Requirement 4

**User Story:** As a user, I want to export car data to a file, so that I can share or backup the inventory information.

#### Acceptance Criteria

1. WHEN the User initiates an export operation THEN the AutoSalon System SHALL create an Export File containing brand, mileage, and price for each Car Record
2. WHEN writing to the Export File THEN the system SHALL format each Car Record on a separate line with fields separated by delimiters
3. IF the Export File cannot be created or written THEN the system SHALL display an error message to the User
4. WHEN the export completes successfully THEN the system SHALL display a confirmation message with the file location

### Requirement 5

**User Story:** As a user, I want to read and display the contents of an exported file, so that I can verify the exported data.

#### Acceptance Criteria

1. WHEN the User requests to view an Export File THEN the AutoSalon System SHALL read the file contents and display them in a readable format
2. IF the specified file does not exist THEN the system SHALL display an error message to the User
3. IF the file cannot be read due to permissions or other errors THEN the system SHALL display an appropriate error message
4. WHEN displaying file contents THEN the system SHALL preserve the original formatting and structure

### Requirement 6

**User Story:** As a user, I want to add new car records to the database, so that I can maintain an up-to-date inventory.

#### Acceptance Criteria

1. WHEN the User provides brand, manufacturer, release date, mileage, and price values and submits the form THEN the AutoSalon System SHALL insert a new Car Record into the PostgreSQL Database
2. WHEN the User attempts to add a Car Record with empty required fields THEN the system SHALL reject the submission and display validation error messages
3. WHEN the User provides invalid data types (non-numeric mileage or price, invalid date format) THEN the system SHALL reject the input and display appropriate error messages
4. WHEN a Car Record is successfully added THEN the system SHALL refresh the table view to display the new record
5. WHEN the database insertion fails THEN the system SHALL display an error message and maintain the current state

### Requirement 7

**User Story:** As a user, I want to delete all cars from a specific manufacturer, so that I can remove discontinued inventory.

#### Acceptance Criteria

1. WHEN the User specifies a manufacturer name and confirms deletion THEN the AutoSalon System SHALL remove all Car Records where the manufacturer field matches the specified value
2. WHEN the specified manufacturer has no matching Car Records THEN the system SHALL display a message indicating no records were deleted
3. WHEN Car Records are successfully deleted THEN the system SHALL refresh the table view to reflect the changes
4. WHEN the deletion operation fails THEN the system SHALL display an error message and maintain the current database state

### Requirement 8

**User Story:** As a user, I want to update the price of cars by brand, so that I can adjust pricing for specific models.

#### Acceptance Criteria

1. WHEN the User specifies a brand name and a new price value and confirms the update THEN the AutoSalon System SHALL modify the price field for all Car Records matching the specified brand
2. WHEN the User provides a non-numeric price value THEN the system SHALL reject the input and display an error message
3. WHEN the specified brand has no matching Car Records THEN the system SHALL display a message indicating no records were updated
4. WHEN Car Records are successfully updated THEN the system SHALL refresh the table view to display the new prices
5. WHEN the update operation fails THEN the system SHALL display an error message and maintain the current database state

### Requirement 9

**User Story:** As a system administrator, I want the application to run in Docker containers with GUI access, so that deployment is consistent and the interface is accessible.

#### Acceptance Criteria

1. WHEN the system is deployed THEN the AutoSalon System SHALL run in a Docker Container with all required dependencies including Qt GUI libraries
2. WHEN the system is deployed THEN the PostgreSQL Database SHALL run in a separate Docker Container
3. WHEN the Docker containers start THEN the AutoSalon System SHALL automatically connect to the PostgreSQL Database using the configured connection parameters
4. WHEN the Docker containers are stopped and restarted THEN the PostgreSQL Database SHALL persist all Car Records using a Docker volume
5. WHEN the application container starts THEN the system SHALL provide GUI access through VNC on port 5900 or web browser on port 6080
6. WHEN the User accesses the application through a web browser THEN the system SHALL display the Qt GUI interface in the browser window

### Requirement 10

**User Story:** As a developer, I want the codebase to follow object-oriented principles, so that the system is maintainable and extensible.

#### Acceptance Criteria

1. WHEN implementing the system THEN the AutoSalon System SHALL separate concerns into distinct classes (Database, Operations, GUI)
2. WHEN implementing the system THEN the system SHALL use encapsulation to hide implementation details and expose clear interfaces
3. WHEN implementing the system THEN the system SHALL organize code into multiple files based on functionality
4. WHEN implementing database operations THEN the system SHALL use a dedicated Database class to manage connections and queries
5. WHEN implementing business logic THEN the system SHALL use an Operations class to encapsulate CRUD operations
