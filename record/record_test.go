package record

import (
	"bytes"
	"encoding/binary"
	"testing"

	"github.com/stretchr/testify/assert"
)

func TestNew(t *testing.T) {
	entry := New()
	assert.NotNil(t, entry)
}

// TestToBytesFromBytes verifies a Record can be serialized and deserialized correctly.
func TestRecordToBytesFromBytes(t *testing.T) {
	var originalPayload uint32 = 31415926
	originalRecord := New()

	bs := make([]byte, 4)
	binary.LittleEndian.PutUint32(bs, originalPayload)
	originalRecord.Data = bs

	var serialized []byte = make([]byte, 20)

	// Serialize record to bytes.
	originalRecord.EncodeTo(serialized)

	// Deserialize record from bytes.
	var deserializedRecord Record
	err := deserializedRecord.DecodeFrom(serialized)

	if assert.Nil(t, err, "Error deserializing Record.") {
		if assert.NotNil(t, deserializedRecord, "Deserialized Record is nil.") {
			deserializedPayload := binary.LittleEndian.Uint32(deserializedRecord.Data)

			assert.EqualValues(t, len(originalRecord.Data), len(deserializedRecord.Data))
			assert.EqualValues(t, 4, deserializedRecord.LengthOfData())
			assert.Equal(t, originalPayload, deserializedPayload)
		}
	}
}

func BenchmarkToBytes(b *testing.B) {
	var originalPayload uint32 = 31415926
	originalRecord := New()

	bs := make([]byte, 4)
	binary.LittleEndian.PutUint32(bs, originalPayload)
	originalRecord.Data = bs

	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		// Serialize record to bytes.
		ToBytes(originalRecord)
		b.SetBytes(1)
	}
}

func BenchmarkWriteTo(b *testing.B) {
	var originalPayload uint32 = 31415926
	originalRecord := New()

	bs := make([]byte, 4)
	binary.LittleEndian.PutUint32(bs, originalPayload)
	originalRecord.Data = bs

	var buffer bytes.Buffer

	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		// Serialize record to bytes.
		originalRecord.WriteTo(&buffer)
		b.SetBytes(1)
	}
}

func BenchmarkEncodeTo(b *testing.B) {
	var originalPayload uint32 = 31415926
	originalRecord := New()

	bs := make([]byte, 4)
	binary.LittleEndian.PutUint32(bs, originalPayload)
	originalRecord.Data = bs

	var buffer []byte = make([]byte, 20)

	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		// Serialize record to bytes.
		originalRecord.EncodeTo(buffer)
		b.SetBytes(1)
	}
}

func BenchmarkFromBytes(b *testing.B) {
	var originalPayload uint32 = 31415926
	originalRecord := New()

	bs := make([]byte, 4)
	binary.LittleEndian.PutUint32(bs, originalPayload)
	originalRecord.Data = bs

	// Serialize record to bytes.
	serialized, _ := ToBytes(originalRecord)

	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		// Deserialize record from bytes.
		FromBytes(serialized)
		b.SetBytes(1)
	}
}

func BenchmarkReadFrom(b *testing.B) {
	var originalPayload uint32 = 31415926
	originalRecord := New()

	bs := make([]byte, 4)
	binary.LittleEndian.PutUint32(bs, originalPayload)
	originalRecord.Data = bs

	// Serialize record to bytes.
	var buffer bytes.Buffer
	originalRecord.WriteTo(&buffer)

	var deserializedRecord Record

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		deserializedRecord.ReadFrom(&buffer)
		b.SetBytes(1)
	}
}

func BenchmarkDecodeFrom(b *testing.B) {
	var originalPayload uint32 = 31415926
	originalRecord := New()

	bs := make([]byte, 4)
	binary.LittleEndian.PutUint32(bs, originalPayload)
	originalRecord.Data = bs

	var buffer []byte = make([]byte, 20)
	originalRecord.EncodeTo(buffer)
	var deserializedRecord Record
	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		// Deserialize record to bytes.
		deserializedRecord.DecodeFrom(buffer)
		b.SetBytes(1)
	}
}
