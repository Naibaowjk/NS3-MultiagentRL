sudo apt-get install build-essential fakeroot dpkg-dev -y
sudo apt-get build-dep git -y
sudo apt-get install libcurl4-openssl-dev -y
cd ~
mkdir source-git
cd source-git/
apt-get source git
cd git-2.*.*/
sed -i -- 's/libcurl4-gnutls-dev/libcurl4-openssl-dev/' ./debian/control
sed -i -- '/TEST\s*=\s*test/d' ./debian/rules
dpkg-buildpackage -rfakeroot -b -uc -us
sudo dpkg -i ../git_*ubuntu*.deb