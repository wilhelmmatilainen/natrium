#include <nan/nan.h>
#include <node.h>
#include <sodium.h>
#include "natrium.h"


namespace natrium {
class SecretBoxEncrypt : public Nan::AsyncWorker
{
public:
	SecretBoxEncrypt(Nan::Callback * callback, char * key, char * message, size_t message_size): Nan::AsyncWorker(callback), _key(key), _message(message), _message_size(message_size), _cipher_size(message_size + crypto_box_NONCEBYTES + crypto_secretbox_MACBYTES) {
		_cipher = new char[_cipher_size];
	}

	void Execute() {
		randombytes_buf((unsigned char *)_cipher, crypto_secretbox_NONCEBYTES);

		if(crypto_secretbox_easy((unsigned char *)_cipher + crypto_box_NONCEBYTES, (unsigned char *)_message, _message_size, (unsigned char *) _cipher, (unsigned char *)_key))
		 SetErrorMessage("Encrypting a message failed");
	}

	void HandleOKCallback()
	{
		Nan::HandleScope scope;
		v8::Local<v8::Value> argv[] = {
			Nan::Null(),
			NATRIUM_BUFFER(_cipher, _cipher_size)
		};
		callback->Call(2, argv);
	}

protected:
	char * _key;
	char * _message;
	size_t _message_size;
	char * _cipher;
	size_t _cipher_size;
};

NAN_METHOD(secretbox_encrypt)
{
	Nan::AsyncQueueWorker(new SecretBoxEncrypt(NATRIUM_CALLBACK(2), NATRIUM_BUFFER_DATA(0), NATRIUM_BUFFER_DATA(1), NATRIUM_BUFFER_SIZE(1)));
}
}
