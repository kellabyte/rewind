package record

import (
	"encoding/binary"
	"io"

	"github.com/rs/xid"
	"gopkg.in/restruct.v1"
)

// Record represents a log record that contains a global id and the byte payload of arbitrary
// record information stored from the consuming application.
//
// The byte format of a Record is as follows.
// 12 byte ID.
// 4 byte int represents length of the data buffer.
// N byte data buffer.
type Record struct {
	// Id represents a unique id of the log entry.
	// 4 byte value representing the seconds since the Unix epoch.
	// 3 byte machine identifier.
	// 2 byte process id.
	// 3 byte counter starting with a random value.
	ID xid.ID

	// Data represents the bytes that make up the log data.
	Data []byte
}

// New returns a new Record instance.
func New() *Record {
	return &Record{
		ID:   xid.New(),
		Data: nil,
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

// LengthOfData returns the size in bytes of the record data buffer.
func (record *Record) LengthOfData() int { return len(record.Data) }

// LengthOfRecord returns the size in bytes of the entire record.
func (record *Record) LengthOfRecord() int { return 12 + 4 + record.LengthOfData() }

// EncodeTo encodes the record to a byte buffer.
func (record *Record) EncodeTo(data []byte) {
	_ = data[16]
	_ = data[record.LengthOfRecord()-1] // early bounds check

	sz := len(record.Data)
	copy(data, record.ID[:])
	data[12] = byte(sz >> 24)
	data[13] = byte(sz >> 16)
	data[14] = byte(sz >> 8)
	data[15] = byte(sz)

	copy(data[16:], record.Data)
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

// DecodeFrom decodes a Record from a byte buffer.
func (record *Record) DecodeFrom(buffer []byte) error {
	copy(record.ID[:], buffer[:12])

	sz := (int(buffer[12]) << 24) | (int(buffer[13]) << 16) | (int(buffer[14]) << 8) | int(buffer[15])

	if cap(record.Data) < sz {
		record.Data = make([]byte, sz)
		copy(record.Data, buffer[16:16+sz])
	} else {
		record.Data = record.Data[:sz]
	}

	return nil
}
