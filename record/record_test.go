package record

import (
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
	originalRecord.Length = 4

	bs := make([]byte, 4)
	binary.LittleEndian.PutUint32(bs, originalPayload)
	originalRecord.Data = bs

	// Serialize record to bytes.
	serialized, err := ToBytes(originalRecord)
	assert.Nil(t, err)

	// Deserialize record from bytes.
	deserializedRecord, err := FromBytes(serialized)

	if assert.Nil(t, err, "Error deserializing Record.") {
		if assert.NotNil(t, deserializedRecord, "Deserialized Record is nil.") {
			deserializedPayload := binary.LittleEndian.Uint32(deserializedRecord.Data)

			assert.EqualValues(t, len(originalRecord.Data), len(deserializedRecord.Data))
			assert.EqualValues(t, 4, deserializedRecord.Length)
			assert.Equal(t, originalPayload, deserializedPayload)
		}
	}
}

func BenchmarkToBytes(b *testing.B) {
	var originalPayload uint32 = 31415926
	originalRecord := New()
	originalRecord.Length = 4

	bs := make([]byte, 4)
	binary.LittleEndian.PutUint32(bs, originalPayload)
	originalRecord.Data = bs

	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		// Serialize record to bytes.
		ToBytes(originalRecord)
	}
}

func BenchmarkFromBytes(b *testing.B) {
	var originalPayload uint32 = 31415926
	originalRecord := New()
	originalRecord.Length = 4

	bs := make([]byte, 4)
	binary.LittleEndian.PutUint32(bs, originalPayload)
	originalRecord.Data = bs

	// Serialize record to bytes.
	serialized, _ := ToBytes(originalRecord)

	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		// Deserialize record from bytes.
		FromBytes(serialized)
	}
}
