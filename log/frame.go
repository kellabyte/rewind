package log

// Frame represents the envelope format of a log Record. It contains version information and
// a Checksum of the log Record to detect Record corruption.
type Frame struct {
	// Flags represents version information about the format.
	Flags uint8

	// Checksum represents the checksum of the Record for detecting corruption.
	Checksum uint32

	// Length represents the length of the Record field in bytes.
	Length uint32

	// Record represents the bytes that represent a serialized Record.
	Record []byte
}

// NewRecord initializes and returns a new Frame instance.
func NewFrame() *Frame {
	return &Frame{
		Flags:  0,
		Length: 0,
		Record: nil,
	}
}
