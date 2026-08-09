#pragma once

// Holds additional data for a given error code
// TODO Additional fields could be added here like severity, etc.
struct ErrorData
{
	const char* Text = "<ERROR TEXT NOT DEFINED IN ERROR REGISTRY>";
};

// A registry for additional error data
// TODO If needed in the future, consider adding a second template parameter
// so we can accept different types, not just ErrorData
template <CppEnum T>
class ErrorRegistry
{
public:
	using Container = std::array<ErrorData, (size_t)T::Count>;

	explicit ErrorRegistry(const Container& container);

	DISABLE_COPY_AND_MOVE(ErrorRegistry);

	const ErrorData& GetErrorData(T errorCode) const;

private:
	Container m_ErrorData;
};

template <CppEnum T>
inline ErrorRegistry<T>::ErrorRegistry(const Container& container)
	: m_ErrorData(container)
{
}

template <CppEnum T>
inline const ErrorData& ErrorRegistry<T>::GetErrorData(T errorCode) const
{
	static ErrorData invalid;

	ReturnIf((int)errorCode < 0 || (size_t)errorCode >= (size_t)m_ErrorData.size(), invalid);

	return m_ErrorData[(size_t)errorCode];
}
