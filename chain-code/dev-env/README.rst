The files in this this folder are based on `github.com/hyperledger/fabric-samples/chaincode-dev-env`

Using dev mode
==============

Normally chaincodes are started and maintained by peer. However in “dev
mode", chaincode is built and started by the user. This mode is useful
during chaincode development phase for rapid code/build/run/debug cycle
turnaround.

We start "dev mode" by leveraging pre-generated orderer and channel artifacts for
a sample dev network.  As such, the user can immediately jump into the process
of compiling chaincode and driving calls.

Terminal 1 - Start the network
------------------------------

.. code:: bash

    docker-compose up

The above starts the network with the ``SingleSampleMSPSolo`` orderer profile and
launches the peer in "dev mode".  It also launches two additional containers -
one for the chaincode environment and a CLI to interact with the chaincode.  The
commands for create and join channel are embedded in the CLI container, so we
can jump immediately to the chaincode calls.

Terminal 2 - Build & start the chaincode
----------------------------------------

.. code:: bash

  docker exec -it chaincode bash

You should see the following:

.. code:: bash

  root@d2629980e76b:/opt/gopath/src/chaincode#

Now, compile your chaincode:

.. code:: bash

  cd {CC_FOLDER_NAME}
  go build

Now run the chaincode:

.. code:: bash

  CORE_PEER_ADDRESS=peer:7051 CORE_CHAINCODE_ID_NAME=mycc:0 ./{CC_FILE_NAME}

The chaincode is started with peer and chaincode logs indicating successful registration with the peer.
Note that at this stage the chaincgoode is not associated with any channel. This is done in subsequent steps
using the ``instantiate`` command.

Terminal 3 - Use the chaincode
------------------------------

Even though you are in ``--peer-chaincodedev`` mode, you still have to install the
chaincode so the life-cycle system chaincode can go through its checks normally.
This requirement may be removed in future when in ``--peer-chaincodedev`` mode.

We'll leverage the CLI container to drive these calls.

.. code:: bash

  docker exec -it cli bash

.. code:: bash

  peer chaincode install -p chaincodedev/chaincode/{CC_FOLDER_NAME} -n mycc -v 0
  peer chaincode instantiate -n mycc -v 0 -c '{"Args":[{"init", RELEVANT ARGS AS COMMA SEP STRING }]}' -C myc


.. code:: bash

  peer chaincode invoke -n mycc -c '{"Args":[{FUNCTION_NAME}, {RELEVANT ARGS AS COMMA SEP STRING }]}' -C myc

Finally, query ``key``.

.. code:: bash

  peer chaincode query -n mycc -c '{"Args":["query","key"]}' -C myc

Testing new chaincode
---------------------

By default, we mount only ``chaincode_example02``.  However, you can easily test different
chaincodes by adding them to the ``chaincode`` subdirectory and relaunching
your network.  At this point they will be accessible in your ``chaincode`` container.

.. Licensed under Creative Commons Attribution 4.0 International License
     https://creativecommons.org/licenses/by/4.0/
