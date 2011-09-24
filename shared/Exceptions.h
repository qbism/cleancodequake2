class Exception
{
protected:
	String message;

public:
	Exception () :
	  message()
	  {
	  };

	Exception (String message) :
	  message(message)
	  {
	  };

	String &Message ()
	{
		return message;
	}
};

class ExceptionIndexOutOfRange : public Exception
{
public:
	ExceptionIndexOutOfRange() :
	  Exception(String("The specified index is out of range"))
	  {
	  };

	ExceptionIndexOutOfRange(String variable) :
	  Exception(String("The variable \"").Concatenate(variable).Concatenate("\" is out of range"))
	  {
	  };
};

class ExceptionNotImplemented : public Exception
{
public:
	ExceptionNotImplemented() :
	  Exception(String("The specified function/method has not been implemented yet."))
	  {
	  };
};
