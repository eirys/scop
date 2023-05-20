/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   image_handler.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/12 10:23:32 by eli               #+#    #+#             */
/*   Updated: 2023/05/18 22:46:13 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// Std
# include <string>
# include <vector>

# include "image_loader.hpp"

namespace scop {

/**
 * Image handler.
 * 
 * Only handles 32-bit images.
*/
class Image {
public:
	/* ========================================================================= */
	/*                                  TYPEDEF                                  */
	/* ========================================================================= */

	typedef		enum ImageType	ImageType;

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	Image(
		const std::string& path,
		ImageType type,
		std::vector<uint32_t>&& pixels,
		size_t width,
		size_t height
	);

	Image(Image&& x) = default;
	~Image() = default;

	Image() = delete;
	Image(const Image& x) = delete;
	Image&	operator=(const Image& x) = delete;

	/* ========================================================================= */

	const std::string&			getPath() const noexcept;
	const uint32_t*				getPixels() const noexcept;
	size_t						getWidth() const noexcept;
	size_t						getHeight() const noexcept;

private:
	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	const std::string			path;
	ImageType					type;
	const std::vector<uint32_t>	pixels;
	size_t						width;
	size_t						height;

}; // class Image

} // namespace scop