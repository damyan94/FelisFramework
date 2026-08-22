#pragma once

namespace Felis
{
// Holds additional data for a given error code
// TODO Additional fields could be added here like severity, etc.
struct ErrorData
{
	const char* Text = "<ERROR TEXT NOT DEFINED IN ERROR REGISTRY>";
};

// A registry for additional error data
// TODO If needed in the future, consider adding a second template parameter
// so we can accept different types, not just ErrorData
template <ErrorCodeEnum T>
class ErrorRegistry
{
public:
	using Container = std::array<ErrorData, (size_t)T::Count>;

	explicit ErrorRegistry(const Container& container);

	FELIS_DISABLE_COPY_AND_MOVE(ErrorRegistry);

	const ErrorData& GetErrorData(T errorCode) const;

private:
	const Container m_ErrorData;
};

template <ErrorCodeEnum T>
inline ErrorRegistry<T>::ErrorRegistry(const Container& container)
	: m_ErrorData(container)
{
}

template <ErrorCodeEnum T>
inline const ErrorData& ErrorRegistry<T>::GetErrorData(T errorCode) const
{
	static ErrorData invalid;

	using UnderlyingType = std::underlying_type_t<T>;
	const auto index	 = static_cast<UnderlyingType>(errorCode);

	if constexpr (std::is_signed_v<UnderlyingType>)
	{
		ReturnIf(index < 0, invalid);
	}

	ReturnIf(static_cast<size_t>(index) >= m_ErrorData.size(), invalid);

	return m_ErrorData[static_cast<size_t>(index)];
}
} // namespace Felis
