package record

import (
	"encoding/binary"

	"github.com/rs/xid"
	"gopkg.in/restruct.v1"
)

// Record represents a log record that contains a global id and the byte payload of arbitrary
// record information stored from the consuming application.
type Record struct {
	// Id represents a unique id of the log entry.
	// 4 byte value representing the seconds since the Unix epoch.
	// 3 byte machine identifier.
	// 2 byte process id.
	// 3 byte counter starting with a random value.
	ID xid.ID

	// Length represents the length of the Data field in bytes.
	Length uint32 `struct:"uint32,sizeof=Data"`

	// Data represents the bytes that make up the log data.
	Data []byte
}

// New returns a new Record instance.
func New() *Record {
	return &Record{
		ID:     xid.New(),
		Length: 0,
		Data:   nil,
	}
}

// ToBytes serializes the provided Record into a byte buffer.
func ToBytes(record *Record) (data []byte, err error) {
	data, err = restruct.Pack(binary.LittleEndian, record)
	return data, err
}

// FromBytes deserializes a byte buffer into a new Record instance.
func FromBytes(data []byte) (*Record, error) {
	var record Record
	err := restruct.Unpack(data, binary.LittleEndian, &record)
	return &record, err
}
