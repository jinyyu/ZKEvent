ZkClient
=======

#### build zk

    yum -y install cppunit-devel ant
    git clone https://github.com/apache/zookeeper.git
    cd zookeeper
    ant compile_jute
    cd src/c
    autoreconf -if
    ./configure --prefix=/usr --disable-shared
    make install

