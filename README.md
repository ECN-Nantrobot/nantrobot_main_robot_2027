
# Main Robot Software Repository

Welcome to the official software development repository for the 2027 Coupe de France de Robotique.

This repository contains all software developed by the **ROS2** and **Firmware** teams. It serves as a central platform for collaborative development, version control, integration, and maintenance of the **main robot's** software.

The repository is organised into two main directories, allowing each team to develop and maintain its respective software while following a common development workflow.

---

## 1. Repository Structure

```text
nantrobot_main_robot_2027/
│
├── ROS2/
│   ├── src/
│   ├── launch/
│   ├── config/
│   └── README.md
│
├── Firmware/
│   ├── src/
│   ├── include/
│   ├── drivers/
│   ├── config/
│   ├── tests/
│   └── README.md
│
├── .github/
│   └── CODEOWNERS
│
├── .gitignore
└── README.md
```

### ROS2

Contains all software developed using ROS2, including:

- ROS2 packages, nodes, and source code.
- Robot control and coordination.
- Navigation and path planning.
- Strategy 
- Launch files and system configurations.

### Firmware

Contains all embedded software developed for the robot's microcontrollers and electronic hardware, including:

- Microcontroller source code.
- Sensor and actuator drivers.
- Motor and low-level control algorithms.
- Hardware communication interfaces.
- Configuration files and hardware-specific settings.
- Firmware testing and validation.

Each directory should contain its own `README.md` providing team-specific documentation, installation instructions, dependencies, and execution procedures.

---

## 2. Getting Started

### Prerequisites

Before contributing, ensure that you have:

- A GitHub account with access to the repository.
- Git installed on your computer.
- GitHub Desktop (recommended for beginners) or a terminal with Git.
- The relevant development environment for your team.

For example, ROS2 developers should have the appropriate ROS2 distribution installed, while Firmware developers should have the required microcontroller toolchains and development environments.

### Cloning the Repository

To get a local copy of the repository, open a terminal and execute:

```bash
git clone https://github.com/ECN-Nantrobot/nantrobot_main_robot_2027.git
```

Navigate into the repository:

```bash
cd nantrobot_main_robot_2027
```

The repository's primary development branch is `develop`. Switch to it and retrieve the latest version:

```bash
git checkout develop
git pull origin develop
```

You are now ready to start contributing.

---

## 3. Development Workflow

To maintain an organised development environment and minimise conflicts between contributors, all development must take place on individual task-specific branches.

The repository follows three levels of branches:

| Branch | Purpose |
|---|---|
| `main` | Contains stable, tested, and approved software. |
| `develop` | Contains the latest integrated development version. |
| Task branches | Used by individual developers to implement features, fixes, and improvements. |

### Branch Naming Convention

All task branches must follow this naming convention:

**ROS2 team:**

```text
ros2/feature-name
ros2/fix-name
```

Examples:

```text
ros2/add-navigation
ros2/implement-obstacle-detection
ros2/fix-odometry
```

**Firmware team:**

```text
firmware/feature-name
firmware/fix-name
```

Examples:

```text
firmware/motor-control
firmware/encoder-driver
firmware/fix-serial-communication
```

Use descriptive names that clearly identify the task being implemented.

---

## 4. How to Contribute

Follow these steps whenever you start working on a new task.

### Step 1: Update Your Local Repository

Before starting any new task, ensure that your local `develop` branch is up to date.

```bash
git checkout develop
git pull origin develop
```

This ensures that your work is based on the latest integrated version of the project.

### Step 2: Create a New Working Branch

Create a new branch for the task assigned to you.

For example, a ROS2 developer implementing navigation would execute:

```bash
git checkout -b ros2/add-navigation
```

A Firmware developer implementing motor control would execute:

```bash
git checkout -b firmware/motor-control
```

Each task should have its own branch. Avoid working directly on `main` or `develop`.

### Step 3: Make Your Changes

Implement your assigned task within the appropriate directory.

- ROS2 developers should primarily work within `ROS2/`.
- Firmware developers should primarily work within `Firmware/`.

Keep your changes focused on the task at hand. Avoid modifying unrelated files.

If your task requires changes to both directories, coordinate with the relevant team before proceeding.

### Step 4: Review and Commit Your Changes

Before committing, check which files have been modified:

```bash
git status
```

Stage the relevant changes:

```bash
git add ROS2/
```

Or, for Firmware:

```bash
git add Firmware/
```

Commit your changes using a clear and descriptive commit message:

```bash
git commit -m "Add navigation module"
```

Examples of good commit messages:

- `Implement motor control`
- `Add encoder driver`
- `Fix odometry calculations`
- `Update serial communication protocol`
- `Add obstacle detection node`

Avoid vague messages such as `Update`, `Changes`, or `New code`.

### Step 5: Push Your Branch to GitHub

Upload your committed changes to the remote repository:

```bash
git push -u origin ros2/add-navigation
```

Replace the branch name with your own task branch.

For subsequent pushes to the same branch, simply execute:

```bash
git push
```

### Step 6: Create a Pull Request

Once your task is ready for integration:

1. Navigate to the repository on GitHub.
2. Open the **Pull Requests** tab.
3. Select **New Pull Request**.
4. Set the base branch to `develop`.
5. Set the compare branch to your task branch.
6. Provide a descriptive title and summary of your changes.
7. Request a review from your team leader.

**All Pull Requests must target `develop`, not `main`**.

### Step 7: Code Review and Integration

Before merging, the relevant team leader or designated reviewer should verify that:

- The implementation meets the task requirements.
- The code has been tested.
- No unrelated files have been modified.
- Existing functionality has not been broken.
- Any relevant documentation has been updated.

Once approved, the Pull Request can be merged into `develop`.

After successful integration, the task branch can be deleted.

---

## 5. Updating Your Working Branch

While you are working on a task, other developers may merge new changes into `develop`.

To incorporate these changes into your working branch, first commit or safely preserve your current work.

Then execute:

```bash
git checkout develop
git pull origin develop
```

Return to your task branch:

```bash
git checkout your-branch-name
```

Merge the latest development changes into your branch:

```bash
git merge develop
```

If Git reports merge conflicts, resolve them before continuing.

Once the merge is complete, you can continue working and push your updated branch when ready.

**Important:** Always ensure that your local `develop` branch is up to date before merging it into your working branch.

---

## 6. General Development Guidelines

To maintain code quality and ensure smooth collaboration, all contributors are expected to follow these guidelines:

1. **Work on task-specific branches:** Never develop directly on `main` or `develop`.

2. **Keep changes organised:** Make changes only to files relevant to your assigned task.

3. **Commit regularly:** Commit meaningful, logically grouped changes rather than waiting until an entire feature is complete.

4. **Use descriptive commit messages:** Clearly communicate what each commit introduces or modifies.

5. **Test before submitting:** Verify your implementation before creating a Pull Request.

6. **Avoid committing generated files:** Build outputs, temporary files, and generated directories should not be uploaded to the repository.

7. **Communicate across teams:** Inform the other team whenever a change affects shared interfaces, communication protocols, or system integration.

8. **Review before merging:** All changes must go through the Pull Request process.

---

## 7. Important Git Commands Reference

| Task | Command |
|---|---|
| Clone repository | `git clone <repository-url>` |
| Check current branch | `git branch` |
| Switch branch | `git checkout <branch>` |
| Update develop | `git pull origin develop` |
| Create new branch | `git checkout -b <branch-name>` |
| Check modified files | `git status` |
| Stage changes | `git add <file-or-folder>` |
| Commit changes | `git commit -m "message"` |
| Push new branch | `git push -u origin <branch-name>` |
| Push subsequent changes | `git push` |
| Merge develop into task branch | `git merge develop` |

---

## 8. Final Notes

The success of this repository depends on consistent collaboration and communication between the ROS2 and Firmware teams.

The `develop` branch serves as the central integration point for both teams, while `main` represents the stable version of the robot's software.

All contributors are expected to follow the development workflow outlined in this document to ensure that changes remain organised, traceable, and easy to integrate.

**Build independently. Collaborate consistently. Integrate reliably.**
