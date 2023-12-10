# Spacecraft Intelligent Capability Sustainable Architecture (SICSA)

The Nezha Architecture (Spacecraft Intelligent Capability Sustainable Architecture, abbreviated as SICSA) is an advanced spacecraft intelligence capability architecture designed specifically to support adaptive scientific exploration. This architecture is based on the CCSDS SOIS architecture principle and implements effective isolation between lower network layers and higher application layers through a convergence layer protocol, thereby promoting the sustainable development of spacecraft intelligence capabilities.

## Software Architecture
SICSA's software architecture adopts a modular design, allowing flexible configuration and expansion to meet the needs of various spacecraft systems and missions. The architecture is divided into multiple layers, including the sub-network layer, application support layer, and application layer, each providing a set of standardized interfaces and services.

## Installation

### Prerequisites
- Ensure your system meets the latest version of software dependencies.
- Suitable for most Linux-based operating systems.

### Installation Steps
1. Clone the repository to your local environment:
   ```
   git clone https://example.com/SICSA.git
   ```
2. Enter the cloned repository directory:
   ```
   cd SICSA
   ```
3. Install necessary dependencies:
   ```
   ./install_dependencies.sh
   ```
4. Compile and build the project:
   ```
   make all
   ```
5. Install to the system:
   ```
   sudo make install
   ```

## Instructions

### Configure the System
1. Open the configuration file `config/sicsa_config.json`.
2. Modify the relevant configuration items according to your system needs.
3. Save and close the configuration file.

### Running the Program
1. Start the SICSA service:
   ```
   sicsa start
   ```
2. Interact with SICSA using the provided command line tools or API.

### Routine Maintenance
- Check logs: SICSA regularly records important system logs and events, stored in the `/var/log/sicsa/` directory.
- Update the system: To get the latest features and security updates, regularly execute the following command:
  ```
  sicsa update
  ```

## Contribution
- Fork the repository.
- Create a new feature branch (Feat_xxx).
- Submit your code.
- Create a Pull Request.
