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
