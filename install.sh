NC='\033[0m'
RED='\033[0;31m'
GREEN='\033[0;32m'

if [ -x "$(command -v apt)" ]; then
  printf "${GREEN}Installing dependencies for debian based distro${NC}\n"
  sudo apt update || printf "${RED}Could not update package list${NC}\n" || exit 1
  sudo apt install -y cmake make g++ libgtk-3-dev libsqlite3-dev || printf "${RED}Could not install dependencies${NC}\n" || exit 1
elif [ -x "$(command -v dnf)" ]; then
  printf "${GREEN}Installing dependencies for rpm based distro${NC}\n"
  sudo dnf update || printf "${RED}Could not update package list${NC}\n" || exit 1
  sudo dnf install cmake make gcc-g++ gtkmm3.0-devel sqlite-devel -y || printf "${RED}Could not install dependencies${NC}\n" || exit 1
elif [ -x "$(command -v pacman)" ]; then
  printf "${GREEN}Installing dependencies for arch based distro${NC}\n"
  sudo pacman -Syu --noconfirm || printf "${RED}Could not update package list${NC}\n" || exit 1
  sudo pacman -S cmake make gcc gtkmm3 sqlite --noconfirm || printf "${RED}Could not install dependencies${NC}\n" || exit 1
elif [ -x "$(command -v yum)" ]; then
  printf "${GREEN}Installing dependencies with yum${NC}\n"
  sudo yum update || printf "${RED}Could not update package list${NC}\n" || exit 1
  sudo yum install cmake make gcc-c++ gtkmm3.0-devel sqlite-devel -y || printf "${RED}Could not install dependencies${NC}\n" || exit 1
else
  printf "${RED}Unsupported package manager, please open a github issue or a pull request${NC}\n"
  exit 1
fi

if [ ! -d "cmake-build-release" ]; then
    mkdir cmake-build-release
fi

cd cmake-build-release || printf "${RED}Could not find cmake-build-release directory${NC}\n" || exit 1
cmake -DCMAKE_BUILD_TYPE=Release .. || printf "${RED}CMake failed${NC}\n" || exit 1
make -j$(($(nproc) - 2)) . || printf "${RED}Make failed${NC}\n" || exit 1

cd .. || printf "${RED}Could not find root directory${NC}\n" || exit 1

cp data/todo.desktop ~/.local/share/applications/todo.desktop || printf "${RED}Could not copy todo.desktop${NC}\n" || exit 1
sed -i "s|path|$PWD|g" ~/.local/share/applications/todo.desktop || printf "${RED}Could not edit todo.desktop${NC}\n" || exit 1

update-desktop-database ~/.local/share/applications
printf "${GREEN}Installation complete${NC}\n"
