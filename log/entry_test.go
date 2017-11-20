package log

import (
	"github.com/stretchr/testify/assert"
	"testing"
)

func TestNewEntry(t *testing.T) {
	entry := NewEntry()
	assert.NotNil(t, entry)
}
