package record

import (
	"encoding/binary"
	"io"

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

// WriteTo writes the record to an io.Writer.
func (record *Record) WriteTo(w io.Writer) (int, error) {
	w.Write(record.ID[:])

	sz := len(record.Data)
	w.Write([]byte{
		byte(sz >> 24),
		byte(sz >> 16),
		byte(sz >> 8),
		byte(sz),
	})

	return w.Write(record.Data)
}

// FromBytes deserializes a byte buffer into a new Record instance.
func FromBytes(data []byte) (*Record, error) {
	var record Record
	err := restruct.Unpack(data, binary.LittleEndian, &record)
	return &record, err
}

// ReadFrom reads from an io.Reader into the current Record.
func (record *Record) ReadFrom(r io.Reader) (int, error) {
	r.Read(record.ID[:])

	var szb [4]byte
	r.Read(szb[:])

	sz := (int(szb[0]) << 24) | (int(szb[1]) << 16) | (int(szb[2]) << 8) | int(szb[3])

	if cap(record.Data) < sz {
		record.Data = make([]byte, sz)
	} else {
		record.Data = record.Data[:sz]
	}
	return r.Read(record.Data)
}
