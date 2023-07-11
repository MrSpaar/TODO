NC='\033[0m'
RED='\033[0;31m'
GREEN='\033[0;32m'

if [ -f "$(grep -Ei 'debian|buntu|mint' /etc/*release)" ]; then
    printf "${GREEN}Installing dependencies for debian based distro${NC}\n"
    sudo apt install cmake make g++ libgtk-3-dev libsqlite3-dev || printf "${RED}Could not install dependencies${NC}\n" || exit 1
elif [ -f "/etc/arch-release" ]; then
    printf "${GREEN}Installing dependencies for arch based distro${NC}\n"
    sudo pacman -S cmake make gcc gtkmm3 libsqlite-devel || printf "${RED}Could not install dependencies${NC}\n" || exit 1
elif [ -f "/etc/fedora-release" ]; then
    printf "${GREEN}Installing dependencies for fedora based distro${NC}\n"
    sudo dnf install cmake make gcc-g++ gtkmm3.0-devel sqlite-devel || printf "${RED}Could not install dependencies${NC}\n" || exit 1
else
    printf "${RED}Unsupported distro, please open a github issue or pull-request${NC}\n"
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

printf "${GREEN}Installation complete${NC}\n"
