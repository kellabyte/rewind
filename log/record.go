package log

import (
	"github.com/rs/xid"
)

// Record represents a log record that contains a global id and the byte payload of arbitrary
// record information stored from the consuming application.
type Record struct {
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

// NewRecord initializes and returns a new Record instance.
func NewRecord() *Record {
	return &Record{
		Id:     xid.New(),
		Length: 0,
		Data:   nil,
	}
}
