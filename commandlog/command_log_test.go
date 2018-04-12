package commandlog

import (
	"testing"

	"github.com/stretchr/testify/assert"
)

func TestNew(t *testing.T) {
	log := NewInMemory()
	assert.NotNil(t, log)
}

func TestOpen(t *testing.T) {
	log := NewInMemory()
	assert.NotNil(t, log)

	err := log.Open("command.log", 0666)
	assert.NoError(t, err)
}

func TestOpenCloseReopen(t *testing.T) {
	log := NewInMemory()
	assert.NotNil(t, log)

	err := log.Open("command.log", 0666)
	assert.NoError(t, err)

	err = log.Close()
	assert.NoError(t, err)

	err = log.Open("command.log", 0666)
	assert.NoError(t, err)
}

func BenchmarkAppend4096(b *testing.B) {
	benchmarkAppend(4096, b)
}

func benchmarkAppend(batchSize int, b *testing.B) {
	var buffer [4096]byte
	log := NewInMemory()
	log.Open("command.log", 0666)

	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		bytes, _ := log.Append(buffer[:])
		log.Sync()
		b.SetBytes(int64(bytes))
	}
}
