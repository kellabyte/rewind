package log

import "github.com/rs/xid"

type Entry struct {
	// Flags represents version information about the log entry.
	Flags uint

	// Id represents a unique id of the log entry.
	// 4 byte value representing the seconds since the Unix epoch.
	// 3 byte machine identifier.
	// 2 byte process id.
	// 3 byte counter starting with a random value.
	Id xid.ID

	// Length represents the length of the Data field in bytes.
	Length uint32

	// Data represents the bytes that make up the log data.
	Data []byte
}

func NewEntry() *Entry {
	return &Entry{
		Flags:  0,
		Id:     xid.New(),
		Length: 0,
		Data:   nil,
	}
}
