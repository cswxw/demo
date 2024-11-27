#include <stdio.h>  // for printf
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <SQLAPI.h> // main SQLAPI++ header
#pragma comment(lib,"sqlapisd.lib")
#pragma comment(lib,"Version.lib")

SAString ReadWholeFile(const char *sFilename)
{
	SAString s;
	char sBuf[32 * 1024];
	FILE *pFile = fopen(sFilename, "rb");

	if (!pFile)
		SAException::throwUserException(-1,
			"Error opening file '%s'\n", sFilename);
	do
	{
		size_t nRead = fread(sBuf, 1, sizeof(sBuf), pFile);
		s += SAString(sBuf, nRead);
	} while (!feof(pFile));

	fclose(pFile);

	return s;
}
static FILE *pFile = NULL;
size_t nTotalBound;
size_t FromFileWriter(
	SAPieceType_t &ePieceType,
	void *pBuf, size_t nLen, void *pAddlData)
{
	if (ePieceType == SA_FirstPiece)
	{
		const char *sFilename = (const char *)pAddlData;
		pFile = fopen(sFilename, "rb");
		if (!pFile)
			SAException::throwUserException(-1, "Can not open file '%s'", sFilename);

		nTotalBound = 0;
	}

	size_t nRead = fread(pBuf, 1, nLen, pFile);
	nTotalBound += nRead;
	// show progress
	printf("%d bytes of file bound\n", nTotalBound);

	if (feof(pFile))
	{
		ePieceType = SA_LastPiece;
		fclose(pFile);
		pFile = NULL;
	}

	return nRead;
}
void MySQL_ShowTables()
{
	SAConnection con;
	try
	{
		con.Connect("localhost@test", "root", "root", SA_MySQL_Client);
		printf("SERVER: %s\n", (const char*)con.ServerVersionString());
		printf("CLIENT VER: %d.%d\n\n",
			(con.ClientVersion() >> 16),
			(con.ClientVersion() & 0x0000FFFF));

		SACommand cmd(&con);
		//cmd.setCommandText("SHOW TABLES");
		cmd.setCommandText("SHOW COLUMNS from t_user");
		cmd.Execute();

		while (cmd.FetchNext())
		{
			for (int i = 1; i <= cmd.FieldCount(); ++i)
			{
				printf("%s: %s, %s\n",
					(const char*)cmd[i].Name(),
					(const char*)cmd[i].asBytes(),
					(const char*)cmd[i].asString());
			}
			printf("\n");
		}
		printf("\n");
	}
	catch (SAException &x)
	{
		// print error message
		printf("%s\n", (const char*)x.ErrText());
	}

}

void connect() {
	SAConnection con; // create connection object
	try
	{
		// connect to database
		// in this example it is Oracle,
		// but can also be Sybase, Informix, DB2
		// SQLServer, InterBase, SQLBase and ODBC
		con.Connect(
			"test",     // database name
			"root",   // user name
			"root",   // password
			SA_MySQL_Client);

		printf("We are connected!\n");

		// Disconnect is optional
		// autodisconnect will ocur in destructor if needed
		con.Disconnect();

		printf("We are disconnected!\n");
		
	}
	catch (SAException &x)
	{
		// SAConnection::Rollback()
		// can also throw an exception
		// (if a network error for example),
		// we will be ready
		try
		{
			// on error rollback changes
			con.Rollback();
		}
		catch (SAException &)
		{
		}
		// print error message
		printf("%s\n", (const char*)x.ErrText());
	}

}

int createTable() {
	SAConnection con; // connection object
	SACommand cmd;    // create command object

	try
	{
		// connect to database (Oracle in our example)
		con.Connect("test", "root", "root", SA_MySQL_Client);
		// associate a command with connection
		// connection can also be specified in SACommand constructor
		cmd.setConnection(&con);

		// create table
		cmd.setCommandText(
			"Create table test_tbl(fid integer, fvarchar20 varchar(20), fblob blob)");
		cmd.Execute();

		// insert value
		cmd.setCommandText(
			"Insert into test_tbl(fid, fvarchar20) values (1, 'Some string (1)')");
		cmd.Execute();

		// commit changes on success
		con.Commit();

		printf("Table created, row inserted!\n");
	}
	catch (SAException &x)
	{
		// SAConnection::Rollback()
		// can also throw an exception
		// (if a network error for example),
		// we will be ready
		try
		{
			// on error rollback changes
			con.Rollback();
		}
		catch (SAException &)
		{
		}
		// print error message
		printf("%s\n", (const char*)x.ErrText());
	}

	return 0;
}
int bindParameter() {

	SAConnection con; // connection object
	SACommand cmd;    // command object

	try
	{
		// connect to database (Oracle in our example)
		con.Connect("test", "root", "root", SA_MySQL_Client);
		// associate a command with connection
		cmd.setConnection(&con);

		// Insert 2 rows
		cmd.setCommandText(
			"Insert into test_tbl(fid, fvarchar20) values(:1, :2)");

		// use first method of binding - param assignment
		cmd.Param(1).setAsLong() = 2;
		cmd.Param(2).setAsString() = "Some string (2)";
		// Insert first row
		cmd.Execute();

		// use second method of binding - stream binding
		cmd << (long)3 << "Some string (3)";
		// Insert second row
		cmd.Execute();

		// commit changes on success
		con.Commit();

		printf("Input parameters bound, rows inserted!\n");
	}
	catch (SAException &x)
	{
		// SAConnection::Rollback()
		// can also throw an exception
		// (if a network error for example),
		// we will be ready
		try
		{
			// on error rollback changes
			con.Rollback();
		}
		catch (SAException &)
		{
		}
		// print error message
		printf("%s\n", (const char*)x.ErrText());
	}

	return 0;
}
int select() {
	SAConnection con; // connection object
	SACommand cmd(
		&con,
		"Select fid, fvarchar20 from test_tbl");    // command object

	try
	{
		// connect to database (Oracle in our example)
		con.Connect("test", "root", "root", SA_MySQL_Client);

		// Select from our test table
		cmd.Execute();
		// fetch results row by row and print results
		while (cmd.FetchNext())
		{
			printf("Row fetched: fid = %ld, fvarchar20 = '%s'\n",
				cmd.Field("fid").asLong(),
				(const char*)cmd.Field("fvarchar20").asString());
		}

		// commit changes on success
		con.Commit();

		printf("Rows selected!\n");
	}
	catch (SAException &x)
	{
		// SAConnection::Rollback()
		// can also throw an exception
		// (if a network error for example),
		// we will be ready
		try
		{
			// on error rollback changes
			con.Rollback();
		}
		catch (SAException &)
		{
		}
		// print error message
		printf("%s\n", (const char*)x.ErrText());
	}

	return 0;
}
int bindBinary() {
	SAConnection con;		// connection object
							// command object
							// update fblob field of our test table
	SACommand cmd(&con,
		"Update test_tbl set fblob = :fblob where fid =:1");

	try
	{
		// connect to database (Oracle in our example)
		con.Connect("test", "root", "root", SA_MySQL_Client);

		// use first method of Long(Lob) binding - as a whole
		cmd.Param(1).setAsLong() = 1;	// fid
		cmd.Param("fblob").setAsBLob() = ReadWholeFile("123.txt");
		// update first row
		cmd.Execute();

		// use second method of binding - user defined callbacks
		cmd.Param(1).setAsLong() = 2;
		cmd.Param("fblob").setAsBLob(
			FromFileWriter,	// our callback to provide blob data from file
			10 * 1024,		// desired size of each piece
			(void*)"123.txt");	// additional param, file name in our case
								// update second row
		cmd.Execute();	// at that moment Library will call user callback when needed

						// commit changes on success
		con.Commit();

		printf("Blob parameter bound, rows updated!\n");
	}
	catch (SAException &x)
	{
		// SAConnection::Rollback()
		// can also throw an exception
		// (if a network error for example),
		// we will be ready
		try
		{
			// on error rollback changes
			con.Rollback();
		}
		catch (SAException &)
		{
		}
		// print error message
		printf("%s\n", (const char*)x.ErrText());
	}

	return 0;
}
size_t nTotalRead;
void IntoFileReader(
	SAPieceType_t ePieceType,
	void *pBuf,
	size_t nLen,
	size_t nBlobSize,
	void *pAddlData)
{
	if (ePieceType == SA_FirstPiece || ePieceType == SA_OnePiece)
	{
		nTotalRead = 0;
		const char *sFilename = (const char *)pAddlData;
		pFile = fopen(sFilename, "wb");
		if (!pFile)
			SAException::throwUserException(-1, "Can not open file '%s' for writing", sFilename);
	}

	fwrite(pBuf, 1, nLen, pFile);
	nTotalRead += nLen;

	// show progress
	printf("%d bytes of %d read\n", nTotalRead, nBlobSize);

	if (ePieceType == SA_LastPiece || ePieceType == SA_OnePiece)
	{
		fclose(pFile);
		pFile = NULL;
	}
}
int fetchBinary() {
	SAConnection con; // connection object
	SACommand cmd(
		&con,
		"Select fblob from test_tbl");    // command object

	try
	{
		// connect to database (Oracle in our example)
		con.Connect("test", "root", "root", SA_MySQL_Client);

		// Usage 1. Read whole BLob(s) into internal buffers
		// Select BLob from our test table
		cmd.Execute();
		// fetch results row by row and print results
		while (cmd.FetchNext())
		{
			// after fetching a row all Long/Lob fields are automatically read into internal buffers
			// just like other data types
			SAString s = cmd.Field("fblob").asBLob();
			printf("Size of BLob is %d bytes\n", s.GetLength());
		}

		// Usage 2. Read BLob in pieces providing user callback for BLob data processing
		// Select blob from our test table
		cmd.Execute();
		// do not automatically read this field into internal buffer (into corresponding SAField object)
		// we will provide a callback for BLob fetching after FetchNext
		cmd.Field("fblob").setLongOrLobReaderMode(SA_LongOrLobReaderManual);

		// fetch results row by row and print results
		SAString sFilename;
		int i = 0;
		while (cmd.FetchNext())
		{
			sFilename.Format("fblob%d.bin", ++i);
			// at that moment all fields are fetched except
			// those that set for manual retrieving
			// read them (fblob in our example) now
			if (!cmd.Field("fblob").isNull())
				cmd.Field("fblob").ReadLongOrLob(
					IntoFileReader,	// our callback to read BLob content into file
					10 * 1024,		// our desired piece size
					(void*)(const char*)sFilename	// additional data, filename in our example
				);
		}

		// commit changes on success
		con.Commit();

		printf("Rows with BLob field fetched!\n");
	}
	catch (SAException &x)
	{
		// SAConnection::Rollback()
		// can also throw an exception
		// (if a network error for example),
		// we will be ready
		try
		{
			// on error rollback changes
			con.Rollback();
		}
		catch (SAException &)
		{
		}
		// print error message
		printf("%s\n", (const char*)x.ErrText());
	}

	return 0;
}
int main(int argc, char* argv[])
{
	
	connect();
	//MySQL_ShowTables();
	//createTable();
	//bindParameter();
	//select();
	//bindBinary();
	//fetchBinary();
	return 0;
}
