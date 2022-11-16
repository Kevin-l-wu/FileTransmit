SafeFileTransmitClient:
	gcc SafeFileTransmitClient.c Protocol.c Common.c FileOperation.c -o SafeFileTransmitClient

SafeFileTransmitServer:
	gcc SafeFileTransmitServer.c Protocol.c Common.c FileOperation.c -o SafeFileTransmitServer

clean:
	rm -rf SafeFileTransmitClient SafeFileTransmitServer
